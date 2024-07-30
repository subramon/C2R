#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "macros.h"
#include "qtypes.h"
#include "compress_struct.h"
#include "aux_compress.h"
#include "convert.h"
#include "decompress.h"

int 
decompress(
    char * pkt, 
    int *ptr_n,
    int *ptr_m,
    Rtype_t **ptr_Rtypes, // [m]
    size_t **ptr_offsets, // [m]
    char **ptr_out_data,
    char **ptr_out_names
    )
{
  int status = 0;
  int n, m1, m2, w;
  qtype_t *qtypes = NULL; 
  Rtype_t *Rtypes = NULL; 
  int *src_flds = NULL; 
  extractor_t *extractors = NULL; 
  size_t *out_offsets = NULL; 
  size_t *in_offsets = NULL; 
  char *in_data = NULL;
  char *out_data = NULL;
  char *out_names = NULL;

  char *bak = pkt;

  n  = ((int *)pkt)[0];
  m1 = ((int *)pkt)[1];
  m2 = ((int *)pkt)[2];

  if ( n <= 0 ) { go_BYE(-1); }
  if ( m1 <= 0 ) { go_BYE(-1); }
  if ( m2 <= 0 ) { go_BYE(-1); }
  pkt += 16; // read n, m, m2, unused
  //===========================================
  qtypes = (qtype_t *)pkt;
  w = roundup_16(m1 * sizeof(qtype_t));
  pkt += w;
  //===========================================
  Rtypes = (Rtype_t *)pkt;
  w = roundup_16(m2 * sizeof(Rtype_t));
  pkt += w;
  //===========================================
  src_flds = (int *)pkt;
  w = roundup_16(m2 * sizeof(int));
  pkt += w;
  status = chk_src_flds(src_flds, m1, m2); cBYE(status);
  //===========================================
  extractors = (extractor_t *)pkt;
  w = roundup_16(m2 * sizeof(extractor_t));
  pkt += w;
  status = chk_extractors(qtypes, Rtypes, src_flds, extractors, m1, m2);
  cBYE(status);
  //===========================================
  out_names = (char *)pkt;
  int idx = 0;
  for ( int nulls_seen = 0; nulls_seen < m2+1; idx++) { // + 1 for df name 
    if ( pkt[idx] == '\0' ) { nulls_seen++; }
  }
  idx++; // jump over last nullc
  idx = roundup_16(idx);
  pkt += idx;
    
  //===========================================
  // get a handle on the input data 
  in_data = pkt; 
  in_offsets = malloc(m2 *sizeof(size_t));
  return_if_malloc_failed(in_offsets);
  memset(in_offsets, 0, m2 *sizeof(size_t));
  int in_len = 0;
  for ( int i = 0; i < m1; i++ ) { 
    if ( i > 0 ) { 
      in_offsets[i] = in_len;
    }
    in_len += roundup_16(n * get_width(qtypes[i]));
  }
/*
  int32_t *I4ptr = (int32_t *)(in_data + in_offsets[0]);
  for ( int i = 0; i < n; i++ ) { 
    printf("%d:%d\n", i, I4ptr[i]);
  }
  int16_t *I2ptr = (int16_t *)(in_data + in_offsets[1]);
  for ( int i = 0; i < n; i++ ) { 
    printf("%d:%d\n", i, I2ptr[i]);
  }
  float *F4ptr = (float *)(in_data + in_offsets[2]);
  for ( int i = 0; i < n; i++ ) { 
    printf("%d:%f\n", i, F4ptr[i]);
  }
  */
  //===========================================
  out_offsets = malloc(m2 *sizeof(size_t));
  return_if_malloc_failed(out_offsets);
  memset(out_offsets, 0, m2 *sizeof(size_t));
  //===========================================
  // determine the size of the output data and set out_offsets
  int out_len = 0;
  for ( int i = 0; i < m2; i++ ) { 
    if ( i > 0 ) { 
      out_offsets[i] = out_len;
    }
    out_len += roundup_16(n * get_Rwidth(Rtypes[i]));
  }
  // allocate output data 
  out_data = malloc(out_len);
  return_if_malloc_failed(out_data); 
  // set the output data 
  for ( int i = 0; i < m2; i++ ) { 
    char *out_ptr = out_data + out_offsets[i];
    int s = src_flds[i];
    qtype_t q = qtypes[s];
    Rtype_t r = Rtypes[i];
    char *in_ptr = in_data + in_offsets[s];
    status = convert(in_ptr, q, r, out_ptr, extractors[i], n); 
    cBYE(status);
  }

  *ptr_n = n;
  *ptr_m = m2;
  *ptr_offsets  = out_offsets; // allocated here
  *ptr_out_data = out_data; // allocated here
  *ptr_Rtypes   = Rtypes; // NOT allocated here
  *ptr_out_names   = out_names; // NOT allocated here
BYE:
  free_if_non_null(in_offsets);
  return status;
}
