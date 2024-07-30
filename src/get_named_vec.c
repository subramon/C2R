#include "incs.h"
#include "aux_rcli.h"
#include "get_named_vec.h"

/* We assume that there exists a named vector in R. As an example,
 * x = c("abc" = 1.2, "defg" = 3.45)
 * We want the keys and the values of x
 * The keys are returned in key_blob. Assume 0 for the null character
 * key_blob = abc0defg0
 * sz_key_blob is the space we have allocated for key_blob
 * We assume that we won't overflow, we error out if we do 
 * The values are returned in vals. In this case,
 * ((double *)vals)[0] = 1.2
 * ((double *)vals)[1] = 3.45
 * n_vals  = 2
 * sz_vals is the space we have allocated for vals.
 * */

int
get_named_vec_F8(
    int sock, // INPUT 
    const char * const name, // INPUT 

    char * restrict key_blob, // [sz_key_blob] 
    uint32_t sz_key_blob,
    uint32_t *ptr_n_key_blob, // 0 < *ptr_n_key_blob <= sz_key_blob

    double * restrict vals, // [sz_vals] 
    uint32_t sz_vals, 
    uint32_t *ptr_n_vals   // 0 < *ptr_n_vals <= sz_vals
    )
{
  int status = 0;
  char hdr[HDR_LEN];
  struct phdr ph;
  size_t payload_len = 0; int nr;
  size_t len_param1 = 0; char *param1 = NULL;
  status = mk_dt_string(name, &param1, &len_param1);  cBYE(status);
  int dt_val, xt_val; uint32_t *uiptr = NULL;
  //-- initialize to 0
  memset(key_blob, 0, sz_key_blob);
  memset(vals, 0, sz_vals);
  //-----------------------------------------
  // calculate payload_len 
  payload_len = len_param1;
  // send header 
  status = mk_hdr("CMD_eval", payload_len, &ph); cBYE(status);
  size_t num_sent = send(sock, &ph, HDR_LEN, 0);
  if ( num_sent != HDR_LEN ) { go_BYE(-1); }
  //----------------------
  // send param1
  num_sent = send(sock, param1, len_param1, 0);
  if ( num_sent != len_param1 ) { go_BYE(-1); }
  free_if_non_null(param1);
  //----------------------
  status = get_buf_from_sock(sock, (char *)&ph, HDR_LEN, &nr); cBYE(status);
  int n1 = ph.len;
  if ( n1 == 0 ) { go_BYE(-1); }
  status = get_buf_from_sock(sock, hdr, HDR_LEN, &nr); cBYE(status);
  uiptr = (uint32_t *)hdr;
  //------------------
  dt_val =  uiptr[0] & 0xFF;
  if ( dt_val  != DT_SEXP ) { go_BYE(-1); }
  int n2 = (uiptr[0] >> 8) - 12; // TODO CHECK
  if ( (int)( n1 - HDR_LEN ) != n2 ) { go_BYE(-1); }
  //------------------------------
  // TODO CHECK FOLLOWING Not sure 
  if ( ( uiptr[2] & 0xFF)  != XT_LIST_TAG ) { go_BYE(-1); }
  //---------------------------------
  // Figure out how many bytes taken up by keys
  xt_val =  uiptr[3] & 0xFF;
  if ( xt_val  != XT_ARRAY_STR ) { go_BYE(-1); }
  uint32_t l_sz_key_blob  = uiptr[3] >> 8; 
  uint32_t n_key_blob = l_sz_key_blob / sizeof(char);
  if ( l_sz_key_blob >= sz_key_blob ) { 
    fprintf(stderr, "l_sz_key_blob/sz_key_blob  = %d/%d\n", 
    l_sz_key_blob, sz_key_blob); 
    fprintf(stderr, "Re-allocation not yet implemented\n");
    go_BYE(-1);
  }
  //---------------------------------
  //-- read keys
  status = get_buf_from_sock(sock, key_blob, l_sz_key_blob, &nr); 
  cBYE(status);
  int chk_n_vals = 0;
  for ( uint32_t i = 0; i <  l_sz_key_blob; i++ ) { 
    if ( key_blob[i] == '\0' ) {
      chk_n_vals++;
    }
  }
#undef PRINT
#ifdef PRINT
  char *cptr = key_blob;
  for ( int j = 0; j < chk_n_vals; j++ ) { 
    printf("Key %d = %s \n", j, cptr);
    int len = strlen(cptr);
    cptr += (len+1);
  }
  printf("========\n");
  printf("chk_n_vals = %d \n", chk_n_vals);
#endif
  //---------------------------------
  //-- read another 12 bytes TODO But WHY????
  status = get_buf_from_sock(sock, hdr, 12, &nr); cBYE(status);
  uiptr = (uint32_t *)hdr;
  // TODO TODO Following are highly suspect
  uint32_t n_vals = chk_n_vals;
  uint32_t l_sz_vals = n_vals * sizeof(double);
  if ( n_vals >= sz_vals ) { 
    fprintf(stderr, "Re-allocation not yet implemented\n");
    go_BYE(-1);
  }
  //---------------------------------
  //--- read values
  status = get_buf_from_sock(sock, (char *)vals, l_sz_vals, &nr); 
  cBYE(status);
  //----- set outputs
  *ptr_n_vals     = n_vals;
  *ptr_n_key_blob = n_key_blob;
BYE:
  free_if_non_null(param1);
  return status;
}
