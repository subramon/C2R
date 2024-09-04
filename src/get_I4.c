#include "incs.h"
#include "aux_rcli.h"
#include "get_I4.h"

int
get_I4(
    int sock,
    const char * const name,
    int * restrict ptr_dval
    )
{
  int status = 0;
  char hdr[HDR_LEN];
  struct phdr ph;
  size_t payload_len = 0; 
  size_t len_param1 = 0; char *param1 = NULL;
  status = mk_dt_string(name, &param1, &len_param1);  cBYE(status);
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
  // 21 = 33 = XT_ARRAY_INT
  // TODO: What do the other bytes mean?
  unsigned int *pp = (unsigned int *)hdr;
  if ( ( pp[0] & 0xFF ) != DT_SEXP ) { go_BYE(-1); }
  // TODO P1 if ( ( pp[1] & 0xFF ) != XT_ARRAY_INT ) { go_BYE(-1); }
  if ( ( pp[1] & 0xFF ) != XT_ARRAY_INT ) { status = -1; goto BYE; }
  uint32_t len = pp[1] >> 8;
  num_left_to_read -= 8;
  if ( num_left_to_read != (int)len ) { go_BYE(-1); }
  int n = num_left_to_read / sizeof(int);
  if ( n != 1 ) { go_BYE(-1); }
  nr = read(sock, ptr_dval, sizeof(int));
  if ( nr == 0 ) { go_BYE(-2); } 
  if ( nr <  0 ) { go_BYE(-3); } 
BYE:
  free_if_non_null(param1);
  return status;
}
