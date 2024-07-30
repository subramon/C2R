#include "incs.h"
#include "aux_rcli.h"
#include "n_df.h"
static uint32_t
n_df(
    int sock,
    const char * const fn,
    const char * const name
    )
{
  int status = 0;
  if ( sock < 0 ) { WHEREAMI; return 0xFFFFFFFF; } 
  char cmd[1024]; uint32_t clen = sizeof(cmd); memset(cmd, 0, clen);
  char *exec_out = NULL; size_t len_out = 0;
  sprintf(cmd, "%s(%s)", fn, name);
  status = exec_str(sock, cmd, &exec_out, &len_out, XT_ARRAY_INT); 
  if ( status != 0 ) { WHEREAMI; return 0xFFFFFFFF; } 
  if ( exec_out == NULL ) { WHEREAMI; return 0xFFFFFFFF; } 
  if ( len_out == 0 ) { WHEREAMI; return 0xFFFFFFFF; } 
  int itmp = ((int *)exec_out)[0];
  if ( itmp < 0 ) { WHEREAMI; return 0xFFFFFFFF; } 
  uint32_t n = (uint32_t)itmp;
BYE:
  free_if_non_null(exec_out); len_out = 0;
  return n;
}
uint32_t
ncols_df(
    int sock,
    const char * const name
    )
{
  return n_df(sock, "length", name);
}
uint32_t
nrows_df(
    int sock,
    const char * const name
    )
{
  return n_df(sock, "nrow", name);
}

