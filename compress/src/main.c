//  gcc -g decompress.c aux_compress.c mk_hdr.c main.c -I../inc/ -I../../inc/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "macros.h"
#include "qtypes.h"
#include "compress_struct.h"
#include "mk_hdr.h"
#include "decompress.h"
#include "aux_compress.h"
// TODO: Need to think about dealing with nulls

int
main()
{
  int status = 0;
  char *pkt = NULL;
  int hdr_len = 0;
  char **data_flds = NULL; 

  char *out_data = NULL;
  Rtype_t *out_Rtypes = NULL;
  size_t *out_offsets = NULL;
  char *out_names = NULL; 
  
  int n; int out_n; // number of rows 
  int m1; // number of input columns 
  int m2; int out_m2; // number of output columns 
  qtype_t *qtypes = NULL;
  Rtype_t *Rtypes = NULL; 
  int *src_flds = NULL;
  extractor_t *extractors = NULL;
  char *df_name = NULL; char **out_flds = NULL; 

  // make the meta data 
  n = 15; // number of rows 
  m1 = 3; // number of input columns 
  m2 = 4; // number of output columns 

  qtypes = malloc(m1 * sizeof(qtype_t));
  qtypes[0] = I4;
  qtypes[1] = I2;
  qtypes[2] = F4;

  Rtypes = malloc(m2 * sizeof(qtype_t));
  Rtypes[0] = R_LOGICAL;
  Rtypes[1] = R_LOGICAL;
  Rtypes[2] = R_INTEGER;
  Rtypes[3] = R_DOUBLE;

  src_flds = malloc(m2 * sizeof(int));
  src_flds[0] = 0;
  src_flds[1] = 0;
  src_flds[2] = 1;
  src_flds[3] = 2;

  df_name = strdup("rsdf");
  out_flds = malloc(m2 * sizeof(char *));
  out_flds[0] = strdup("rfld1");
  out_flds[1] = strdup("rfld2");
  out_flds[2] = strdup("rfld3");
  out_flds[3] = strdup("rfld4");

  extractors = malloc(m2 * sizeof(extractor_t));
  extractors[0].extractor_enum = EXTRACT_BITS;
  extractors[0].extract_spec.bit_spec.start = 0;
  extractors[0].extract_spec.bit_spec.stop = 1;

  extractors[1].extractor_enum = EXTRACT_BITS;
  extractors[1].extract_spec.bit_spec.start = 1;
  extractors[1].extract_spec.bit_spec.stop = 2;

  extractors[2].extractor_enum = DEFAULT_EXTRACTOR;

  extractors[3].extractor_enum = DEFAULT_EXTRACTOR;

  data_flds = malloc(m1 * sizeof(char *));
  data_flds[0] = malloc(n * sizeof(int32_t));
  data_flds[1] = malloc(n * sizeof(int16_t));
  data_flds[2] = malloc(n * sizeof(float));

  int32_t *I4ptr = (int32_t *)data_flds[0];
  int16_t *I2ptr = (int16_t *)data_flds[1];
  float *F4ptr   = (float   *)data_flds[2];

  for ( int i = 0; i < n; i++ ) { 
    I2ptr[i] = i+1;
    I4ptr[i] = 10*(i+1);
    F4ptr[i] = 100*(i+1);
  }
  pkt = mk_hdr(n, m1, m2, qtypes, Rtypes, src_flds, extractors, 
      df_name, (const char ** const)out_flds, &hdr_len); 
  if ( pkt == NULL ) { WHEREAMI; go_BYE(-1); }
  status = add_data(pkt+hdr_len, (const char ** const)data_flds, 
      m1, n, qtypes);

  status = decompress(pkt,  &out_n, &out_m2, &out_Rtypes, &out_offsets,
      &out_data, &out_names);
  cBYE(status);
  // Check decompression
  if ( n != out_n ) { go_BYE(-1); }
  if ( m2 != out_m2 ) { go_BYE(-1); }
  // Check data 
  char *tmp = out_data + out_offsets[3];
  double *F8ptr = (double *)tmp;
  for ( int i = 0; i < n; i++ ) { 
    printf("3:%d:%lf\n", i, F8ptr[i]);
  }
  tmp = out_data + out_offsets[2];
  I4ptr = (int32_t *)tmp;
  for ( int i = 0; i < n; i++ ) { 
    printf("2:%d:%d\n", i, I4ptr[i]);
  }
  fprintf(stderr, "Test completed successfully\n");
BYE:
  free_if_non_null(pkt);
  free_if_non_null(qtypes);
  free_if_non_null(Rtypes);
  free_if_non_null(src_flds);
  free_if_non_null(extractors);
  if ( out_flds != NULL ) { 
    for ( int i = 0; i < m2; i++ ) { 
      free_if_non_null(out_flds[i]);
    }
    free_if_non_null(out_flds);
  }
  free_if_non_null(df_name);
  if ( data_flds != NULL ) { 
    for ( int i = 0; i < m1; i++ ) { 
      free_if_non_null(data_flds[i]);
    }
    free_if_non_null(data_flds);
  }
  //--------------------------
  free_if_non_null(out_data);
  // NOT allocated. free_if_non_null(out_Rtypes);
  // NOT allocated. free_if_non_null(out_names);
  free_if_non_null(out_offsets);
  //--------------------------
  return status = 0;
}
/*
On the input side, meta data that we need to know is
n1, the number of rows
m1, the number of columns
widths[m]

Data is just a concatenation of the columns

On the output side, the meta data that we need to know is
n2 (same as n1)
m2, the number of columns
qtypes[m2], where qtypes[i] can be one of F8, I4, I1 corresponding to 
XT_ARRAY_DOUBLE
XT_ARRAY_INT
XT_ARRAY_BOOL

source[m2], where 0 <= source[i] < m1 
mechanism[m2] is of type extractor_t

*/
