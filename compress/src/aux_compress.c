#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "macros.h"
#include "qtypes.h"
#include "compress_struct.h"
#include "aux_compress.h"
int
get_Rwidth(
    Rtype_t rtype
    )
{
  switch ( rtype ) { 
    case R_DOUBLE: return sizeof(double); break; 
    case R_LOGICAL: return sizeof(int8_t); break; 
    case R_INTEGER: return sizeof(int32_t); break; 
    default : return -1; break;
  }
}

int
get_width(
    qtype_t qtype
    )
{
  switch ( qtype ) { 
    case I1: return sizeof(int8_t); break; 
    case I2: return sizeof(int16_t); break; 
    case I4: return sizeof(int32_t); break; 
    case I8: return sizeof(int64_t); break; 

    case F4 : return sizeof(float); break; 
    case F8 : return sizeof(double); break; 

    case UI1: return sizeof(uint8_t); break; 
    case UI2: return sizeof(uint16_t); break; 
    case UI4: return sizeof(uint32_t); break; 
    case UI8: return sizeof(uint64_t); break; 
    default : return -1; break;
  }
}

int
roundup_16(
    int n
    )
{
  uint32_t rem = n & 0xF;
  if ( rem != 0 )  { 
    uint32_t quot = n >> 4;
    quot++;
    n = quot <<  4;
  }
  return n;
}

int 
mk_hdr_len(
    int m1,
    int m2,
    const char * const df_name,
    const char ** const out_flds
    )
{

  int hdr_len = 0;
  hdr_len += sizeof(int); // n
  hdr_len += sizeof(int); // m1
  hdr_len += sizeof(int); // m2
  hdr_len = roundup_16(hdr_len); 

  hdr_len += m1 * sizeof(qtype_t); // qtypes
  hdr_len = roundup_16(hdr_len); 

  hdr_len += m2 * sizeof(Rtype_t); // Rtypes
  hdr_len = roundup_16(hdr_len); 

  hdr_len += m2 * sizeof(int); // src_flds
  hdr_len = roundup_16(hdr_len); 

  hdr_len += m2 * sizeof(extractor_t); // extractors
  hdr_len = roundup_16(hdr_len); 

  hdr_len += strlen(df_name) + 1; // +1 for nullc
  for ( int i = 0; i < m2; i++ ) { 
    hdr_len += strlen(out_flds[i]) + 1; // +1 for nullc
  }
  hdr_len = roundup_16(hdr_len); 

  return hdr_len;
}
char *
set_n(
    char *pkt, 
    int n, 
    int m1, 
    int m2
    )
{
  int *x = (int *)pkt;
  x[0] = n;
  x[1] = m1;
  x[2] = m2;
  int l = roundup_16(sizeof(n) + sizeof(m1) + sizeof(m2)); 
  pkt += l;
  return pkt;
}

char *
set_q(
    char *pkt, 
    const qtype_t *const qtypes, 
    int m
    )
{
  int w = m * sizeof(qtype_t);
  memcpy(pkt, qtypes, w);
  int l = roundup_16(w); 
  pkt += l;
  return pkt;
}

char *
set_R(
    char *pkt, 
    const Rtype_t *const rtypes, 
    int m
    )
{
  int w = m * sizeof(Rtype_t);
  memcpy(pkt, rtypes, w);
  int l = roundup_16(w); 
  pkt += l;
  return pkt;
}

char *
set_s(
    char *pkt, 
    const int *const src_flds, 
    int m
    )
{
  int w = m * sizeof(int);
  memcpy(pkt, src_flds, w);
  int l = roundup_16(w); 
  pkt += l;
  return pkt;
}

char *
set_e(
    char *pkt, 
    const extractor_t *const extractors, 
    int m
    )
{
  int w = m * sizeof(extractor_t);
  memcpy(pkt, extractors, w);
  int l = roundup_16(w); 
  pkt += l;
  return pkt;
}

char *
set_f(
    char *pkt, 
    const char * const df_name,
    const char ** const out_flds,
    int m
    )
{
  char *bak  = pkt; 
  int l = strlen(df_name); 
  int n = 0;
  memcpy(pkt, df_name, l);
  pkt += (l+1);
  n += l+1;

  for ( int i = 0; i < m; i++ ) { 
    l = strlen(out_flds[i]); 
    memcpy(pkt, out_flds[i], l);
    pkt += (l+1);
    n  += (l+1);
  }
  n = roundup_16(n); 
  pkt = bak + n;
  return pkt;
}

int
chk_src_flds(
    int *src_flds, // [m2]
    int m1,
    int m2
    )
{
  int status = 0;
  for ( int i = 0; i < m2; i++ ){ 
    if ( ( src_flds[i] < 0 ) || ( src_flds[i] >= m1 ) )  {
      go_BYE(-1);
    }
  }
BYE:
  return status;
}

int
chk_extractors(
    qtype_t *qtypes,
    Rtype_t *Rtypes,
    int *src_flds,
    extractor_t *extractors,
    int m1,
    int m2
    )
{
  int status = 0;
  for ( int i = 0; i < m2; i++ ) { 
    Rtype_t r = Rtypes[i];
    int s = src_flds[i];
    qtype_t q = qtypes[s];
    extractor_enum_t e = extractors[i].extractor_enum;
    switch ( e ) { 
      case EXTRACT_BITS :
        if ( ! ( ( q == I1 ) || ( q == UI1 ) ||
            ( q == I2 ) || ( q == UI2 ) ||
            ( q == I4 ) || ( q == UI4 ) ||
            ( q == I8 ) || ( q == UI8 ) ) ) {
          go_BYE(-1);
        }
        int start = extractors[i].extract_spec.bit_spec.start;
        int  stop = extractors[i].extract_spec.bit_spec.stop;
        switch ( r ) {
          case R_LOGICAL : 
          if ( stop - start != 1 ) { go_BYE(-1); }
          break;
          case R_INTEGER : 
          if ( (stop - start) >= 32 ) { go_BYE(-1); }
          break;
          default : 
          go_BYE(-1);
          break;
        }
        if ( start < 0 ) { go_BYE(-1); }
        if ( start >= stop ) { go_BYE(-1); }
        switch ( q ) { 
          case I1 : case UI1 : if ( stop > 8 )  { go_BYE(-1); } break; 
          case I2 : case UI2 : if ( stop > 16 ) { go_BYE(-1); } break; 
          case I4 : case UI4 : if ( stop > 32 ) { go_BYE(-1); } break; 
          case I8 : case UI8 : if ( stop > 64 ) { go_BYE(-1); } break; 
          default : go_BYE(-1); break; 
        }
        break;
      case FP_TO_F8 :
        printf("to be implemented\n"); // TODO 
        break;
      case DEFAULT_EXTRACTOR :
        printf("nothing to check \n"); 
        break;
      default : 
        go_BYE(-1);
        break;
    }
  }
BYE:
  return status;
}
