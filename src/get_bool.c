#include "incs.h"
#include "aux_rcli.h"
#include "get_bool.h"

// Assume that is a boolean "name" in R. We get its value in *ptr_x

int
get_bool(
    int sock, // INPUT 
    const char * const name, // INPUT 
    bool *ptr_x
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out = 0;

  if ( sock     <= 0    ) { go_BYE(-1); } 
  if ( name     == NULL ) { go_BYE(-1); } 
  status = exec_str(sock, name, &exec_out, &len_out, XT_ARRAY_BOOL); 
  if ( exec_out == NULL ) { go_BYE(-1); }
  if ( len_out != 8 ) { go_BYE(-1); }

  int num_elements = ((int8_t *)exec_out)[0];
  if ( num_elements != 1 ) { go_BYE(-1); }
  *ptr_x = ((uint32_t *)exec_out)[1] & 0x1;
BYE:
  free_if_non_null(exec_out);
  return status;
}
// We set the value of a boolean "name" in R with the value "x"
int
set_bool(
    int sock, // INPUT 
    const char * const name, // INPUT 
    bool x
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out = 0; char *Rcmd = NULL; 
  int len = strlen(name) + strlen(" <- FALSE ") + 16;

  Rcmd = malloc(len); return_if_malloc_failed(Rcmd);
  if ( x ) { 
    sprintf(Rcmd, "%s <- TRUE ", name);
  }
  else {
    sprintf(Rcmd, "%s <- FALSE ", name);
  }
  status = exec_str(sock, Rcmd, &exec_out, &len_out, -1); cBYE(status);
BYE:
  free_if_non_null(exec_out);
  free_if_non_null(Rcmd);
  return status;
}
