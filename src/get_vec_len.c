#include "incs.h"
#include "aux_rcli.h"
#include "get_vec_len.h"

// Assume that is a boolean "name" in R. We get its value in *ptr_x

int
get_vec_len(
    int sock, // INPUT 
    const char * const name, // INPUT 
    int *ptr_n
    )
{
  int status = 0;
  char *Rcmd = NULL;
  char *exec_out = NULL; size_t len_out = 0;
  if ( sock <  0 ) { go_BYE(-1); } 
  if ( name == NULL ) { go_BYE(-1); } 

  *ptr_n = -1; 
  int len = strlen(name) + strlen("length()") + 16;
  Rcmd = malloc(len); return_if_malloc_failed(Rcmd); 

  sprintf(Rcmd, "length(%s)", name); 
  status = exec_str(sock, Rcmd, &exec_out, &len_out, XT_ARRAY_INT); 
  cBYE(status);
  if ( ( exec_out == NULL ) || ( len_out == 0 ) ) { go_BYE(-1); }
  *ptr_n = ((int *)exec_out)[0];
BYE:
  free_if_non_null(exec_out);
  free_if_non_null(Rcmd);
  return status;
}
