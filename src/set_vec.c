#include "incs.h"
#include "aux_rcli.h"
#include "set_vec.h"

/* Given a vector of "n" elements, stored at address "data"
 * and of type "qtype", store this as vector in R with name "name"
 */
int
set_vec(
    int sock,
    const char * const name,
    const char * const qtype,
    void *data, // [n]
    uint32_t n,
    uint32_t in_width // only when qtype == SC
    )
{
  int status = 0;
  struct phdr ph; 
  size_t payload_len = 0;
  size_t len_param1 = 0; char *param1 = NULL;
  size_t len_param2 = 0; char *param2 = NULL;
  uint32_t width = 0; uint32_t xt_val = XT_NULL;
  char *alt_data = NULL;
  
  if ( sock < 0 ) { go_BYE(-1); }
  if ( ( name  == NULL ) || ( *name  == '\0' ) ) { go_BYE(-1); }
  if ( ( qtype == NULL ) || ( *qtype == '\0' ) ) { go_BYE(-1); }
  if ( data == NULL ) { go_BYE(-1); }
  if ( n == 0 ) { go_BYE(-1); }
  //-------------------------------------
  if ( strcmp(qtype, "F8") == 0 ) { 
    width = sizeof(double);
    xt_val = XT_ARRAY_DOUBLE;
  }
  else if ( strcmp(qtype, "I4") == 0 ) { 
    width = sizeof(int32_t);
    xt_val = XT_ARRAY_INT;
  }
  else if ( strcmp(qtype, "I1") == 0 ) { 
    width = sizeof(int8_t);
    xt_val = XT_ARRAY_BOOL;
  }
  else if ( strcmp(qtype, "SC") == 0 ) { 
    if ( in_width == 0 ) { go_BYE(-1); } 
    width = in_width;
    xt_val = XT_ARRAY_STR;
  }
  else {
    go_BYE(-1);
  }
  //-------------------------------------
  status = mk_dt_string(name, &param1, &len_param1);  cBYE(status);
  //-----------------------------------------
  // create param2 
  int len_hdr_param2 = 4 + 4;
  if ( xt_val == XT_ARRAY_BOOL ) { 
    len_hdr_param2 += 4;  // Reason for +4 listed below
    // #define XT_ARRAY_BOOL    36 /* P  data: int(n),byte,byte,... */
  }
  uint32_t cum_len = n*width;
  if ( strcmp(qtype, "SC") == 0 ) { 
    cum_len = 0;
    for ( uint32_t i = 0; i < n; i++ ) { 
      uint32_t len = strlen((char *)data + (i*width));
      if ( len >= width ) { go_BYE(-1); } 
      cum_len += len;
      cum_len ++; // for nullc
    }
    uint32_t uitmp = cum_len / 8;
    if ( (uitmp * 8) != cum_len ) { 
      cum_len = (uitmp+1)*8;
    }
    alt_data = malloc(cum_len); memset(alt_data, 0, cum_len);
    uint32_t idx = 0;
    for ( uint32_t i = 0; i < n; i++ ) { 
      uint32_t len = strlen((char *)data + (i*width));
      memcpy(alt_data+idx, (char *)data + (i*width), len);
      idx += len;
      idx++; // for nulc
    }
    // Explanation of 'X' padding provided by Simon
    /* 
All SEXPs must be padded to 4-byte boundaries (to avoid misalligned memory access - x86/x86_64 will survive it with performance penalty, but other archs fail), so if you need to pad XT_ARRAY_STR then you don't pad with NUL, because there would be no way to distinguish it from valid, empty strings. So in you example, you pad it to let's say "a0bc0def0XXX" so the length is a multiple of 4 (Rserve itself pads with 0x01).
*/
    for ( uint32_t i = idx; i < cum_len; i++ ) {
      alt_data[i] = 'X';
    }
  }
  len_param2 = len_hdr_param2 + cum_len;
  status = posix_memalign((void **)&param2, 16,  len_param2); cBYE(status);
  return_if_malloc_failed(param2);
  memset(param2, 0, len_param2);
  uint32_t *uiptr = (uint32_t *)param2;
  // TODO Note sure about this len_param2 thingy below
  uiptr[0] = (len_param2 << 8) | DT_SEXP;
  uiptr[1] = (len_param2 << 8) | xt_val;
  if ( ( xt_val == XT_ARRAY_DOUBLE) || ( xt_val == XT_ARRAY_INT ) ) { 
    uiptr[1] = ((n*width) << 8) | xt_val;
  }
  if ( xt_val == XT_ARRAY_STR ) { 
    uiptr[1] = (cum_len << 8) | xt_val; // TODO P0 EXPERIMENTAL
  }
  
  if ( xt_val == XT_ARRAY_BOOL ) { 
    uiptr[2] = n;
  }

  // calculate payload_len 
  payload_len = len_param1 + len_param2;
  // send header 
  status = mk_hdr("CMD_setSEXP", payload_len, &ph); cBYE(status);
  size_t num_sent = send(sock, &ph, HDR_LEN, 0);
  if ( num_sent != HDR_LEN ) { go_BYE(-1); }
  //----------------------
  // send param1
  num_sent = send(sock, param1, len_param1, 0);
  if ( num_sent != len_param1 ) { go_BYE(-1); }
  free_if_non_null(param1);
  //----------------------
  // send param2
  bool use_memcpy = false;
  if ( use_memcpy ) {
    char *cptr = param2 + len_hdr_param2;
    memcpy(cptr, data, n * width);
    num_sent = send(sock, param2, len_param2, 0);
    if ( num_sent != len_param2 ) { go_BYE(-1); }
  }
  else {
    num_sent = send(sock, param2, len_hdr_param2, 0);
    if ( (int)num_sent != len_hdr_param2 ) { go_BYE(-1); }
    if ( strcmp(qtype, "SC") == 0 ) { 
      num_sent = send(sock, alt_data, cum_len, 0);
    }
    else {
      num_sent = send(sock, data, (n*width), 0);
    }
    if ( num_sent != cum_len ) { go_BYE(-1); }
  }
  free_if_non_null(param2);
  //----------------------
 
  status = read_hdr(sock, &ph); cBYE(status);
  if ( ph.len != 0 ) { go_BYE(-1); }
  // TODO: I am getting ph.cmd = 65537. What does this mean?
  /*
  uiptr = (uint32_t *)hdr;
  uint32_t r_status  = uiptr[0] >> 24;
  if ( r_status != 0 ) { go_BYE(-1); }
  */
BYE:
  free_if_non_null(param1);
  free_if_non_null(param2);
  free_if_non_null(alt_data);
  return status;
}
