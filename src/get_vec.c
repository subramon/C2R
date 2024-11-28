#include "incs.h"
#include "aux_rcli.h"
#include "get_vec.h"
/* Assume that there is a vector in R with name "name"
 * Store the length of the vector in "*ptr_n"
 * Store the data of the vector in "*ptr_data"
 * Note that this functio mallocs space for data 
 * TODO: Generalize for different types of vectors
 * */
int
get_vec(
    int sock,
    const char * const name,
    const char * const qtype,
    char ** restrict ptr_data, // [n]
    int * restrict ptr_n
    )
{
  int status = 0;
  char hdr[HDR_LEN];
  struct phdr ph;
  size_t payload_len = 0; int num_read;
  size_t len_param1 = 0; char *param1 = NULL;
  status = mk_dt_string(name, &param1, &len_param1);  cBYE(status);
  uint32_t width = 0;  char *data = NULL; size_t sz = 0;

  if ( name == NULL  ) { go_BYE(-1); }
  if ( qtype == NULL  ) { go_BYE(-1); }
  if ( strcmp(qtype, "F8") == 0 ) {
    width = sizeof(double);
  }
  else if ( strcmp(qtype, "I4") == 0 ) {
    width = sizeof(int32_t);
  }
  else if ( strcmp(qtype, "I1") == 0 ) {
    width = sizeof(int8_t);
  }
  else if ( strcmp(qtype, "SC") == 0 ) { 
    width = 1; 
  }
  else {
    go_BYE(-1);
  }
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
  status = read_hdr(sock, &ph); cBYE(status);
  int num_left_to_read = ph.len;
  //-----------------------------------
  int nr = read(sock, hdr, 8);
  if ( nr == 0 ) { go_BYE(-2); } 
  if ( nr <  0 ) { go_BYE(-3); } 
  if ( nr != 8 ) { go_BYE(-1); }
  // hdr contains 0a 14 20 00 21 10 20 00
  // 0a = 10 = DT_SEXP
  // 21 = 33 = XT_ARRAY_DOUBLE
  // TODO: What do the other bytes mean?
  unsigned int *pp = (unsigned int *)hdr;
  if ( ( pp[0] & 0xFF ) != DT_SEXP ) { go_BYE(-1); }
  if ( strcmp(qtype, "F8") == 0  ) { 
    if ( ( pp[1] & 0xFF ) != XT_ARRAY_DOUBLE ) { go_BYE(-1); }
  }
  else if ( strcmp(qtype, "I4") == 0  ) { 
    if ( ( pp[1] & 0xFF ) != XT_ARRAY_INT ) { go_BYE(-1); }
  }
  else if ( strcmp(qtype, "SC") == 0 ) { 
    if ( ( pp[1] & 0xFF ) != XT_ARRAY_STR ) { go_BYE(-1); }
  }
  else if ( strcmp(qtype, "I1") == 0  ) { 
    if ( ( pp[1] & 0xFF ) != XT_ARRAY_BOOL ) { go_BYE(-1); }
  }
  else {
    go_BYE(-1);
  }
  uint32_t len = pp[1] >> 8;
  num_left_to_read -= 8;
  if ( num_left_to_read != (int)len ) { go_BYE(-1); }
  if ( strcmp(qtype, "I1") == 0  ) { // reason for this special caase below 
    int itmp;
    status = get_buf_from_sock(sock, (char *)&itmp, sizeof(int), &num_read);
    cBYE(status);
    num_left_to_read -= sizeof(int); 
    // #define XT_ARRAY_BOOL    36 /* P  data: int(n),byte,byte,... */
    // See set_vec.c as well
  }
  size_t n = num_left_to_read / width;
  // if ( n * width != len ) { go_BYE(-1); }
  // TODO TODO n--; //  TODO TODO TODO P1 P1 WHY is this needed?
  // I think above is done because server pads with extra 8 bytes at end

  sz = n * width; 
  data = malloc(sz); return_if_malloc_failed(data);
  status = get_buf_from_sock(sock, (char *)data, sz, &num_read);
  cBYE(status);
  *ptr_data = data;
  *ptr_n = n;
BYE:
  if ( status < 0 ) { free_if_non_null(data);*ptr_data = NULL;*ptr_n = 0; }
  free_if_non_null(param1);
  return status;
}
