#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "macros.h"
#include "qtypes.h"
#include "compress_struct.h"
#include "aux_compress.h"
#include "mk_hdr.h"

//--- make space for data
char *
mk_hdr(
    int n,
    int m1,
    int m2,
    const qtype_t * const qtypes, // [m1]
    const Rtype_t * const Rtypes, // [m2]
    const int * const src_flds, // [m2]
    const extractor_t * const extractors, // [m2]
    const char * const df_name, 
    const char ** const out_flds, // [m2] 
    int *ptr_hdr_len
    )
{
  int status = 0;
  char *pkt = NULL, *bak = NULL; int l; size_t w;
  if ( n <= 0 ) { go_BYE(-1); }
  if ( m1 <= 0 ) { go_BYE(-1); }
  if ( m2 <= 0 ) { go_BYE(-1); }
  if ( qtypes == NULL ) { go_BYE(-1); }
  int hdr_len = mk_hdr_len(m1, m2, df_name, out_flds);
  //--- make space for data
  int payload_len = 0;
  for ( int i = 0; i < m1; i++ ) { 
    int width = get_width(qtypes[i]);
    if ( width <= 0 ) { go_BYE(-1); }
    int len = n * width;
    len = roundup_16(len); 
    payload_len += len;
  }
  int pkt_len = hdr_len + payload_len;
  bak = pkt = malloc(pkt_len); memset(pkt, 0, pkt_len);
  pkt = set_n(pkt, n, m1, m2);
  pkt = set_q(pkt, qtypes, m1);
  pkt = set_R(pkt, Rtypes, m2);
  pkt = set_s(pkt, src_flds, m2);
  pkt = set_e(pkt, extractors, m2);
  pkt = set_f(pkt, df_name, out_flds, m2);
  if ( (pkt-bak)  != hdr_len ) { go_BYE(-1); }

  *ptr_hdr_len = hdr_len;
BYE:
  if ( status < 0 ) { return NULL; } else { return bak; }
}
int
add_data(
    char *pkt, 
    const char **const data_flds, 
    int m, 
    int n, 
    const qtype_t *const qtypes
    )
{
  int status = 0;
  // now place the data into the packet
  for ( int i = 0; i < m; i++ ) { 
    int width = get_width(qtypes[i]);
    memcpy(pkt, data_flds[i], n * width);
    int len = roundup_16(n * width);
    pkt += len;
  }
BYE:
  return status;
}
