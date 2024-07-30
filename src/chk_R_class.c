#include "incs.h"
#include "aux_rcli.h"
#include "chk_R_class.h"
bool
chk_R_class(
    int sock,
    const char * const name,
    const char * const class
    )
{
  int status = 0;
  char cmd[1024]; uint32_t clen = sizeof(cmd); memset(cmd, 0, clen);
  char *exec_out = NULL; size_t len_out = 0;
  if ( sock <= 0 ) { WHEREAMI; return false; }
  if ( name  == NULL ) { WHEREAMI; return false; }
  if ( class == NULL ) {  WHEREAMI; return false; }
  // Check that data frame created
  free_if_non_null(exec_out); len_out = 0;
  sprintf(cmd, "class(%s)", name);
  status = exec_str(sock, cmd, &exec_out, &len_out, XT_ARRAY_STR); 
  cBYE(status);
  if ( ( exec_out == NULL ) || ( len_out == 0 ) ) { go_BYE(-1); }
  if ( strcmp(exec_out, class) == 0 )  { 
    free_if_non_null(exec_out); len_out = 0;
    return true; 
  }
BYE:
  free_if_non_null(exec_out); len_out = 0;
  return false; 
}
