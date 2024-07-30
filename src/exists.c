#include "incs.h"
#include "aux_rcli.h"
#include "exists.h"
int
exists2(
    int sock, // INPUT 
    const char * const name, // INPUT 
    bool *ptr_x
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out = 0;
  char *Rcmd = NULL; 

  if ( sock <  0 ) { go_BYE(-1); } 
  if ( name == NULL ) { go_BYE(-1); } 
  int len = strlen(name) + strlen("exists(\"\")") + 16;
  Rcmd = malloc(len); return_if_malloc_failed(Rcmd);
  memset(Rcmd, 0, len);
  sprintf(Rcmd, "exists(\"%s\")", name);
  status = exec_str(sock, Rcmd, &exec_out, &len_out, XT_ARRAY_BOOL); 
  if ( ( exec_out == NULL ) || ( len_out == 0 ) ) { go_BYE(-1); }
  // TODO: Following needs more thought
  *ptr_x  = exec_out[4]; // explanation below. 
  /* Header response for false and true is shown below 
      01 00 00 00 00 ff ff ff
      01 00 00 00 01 ff ff ff
    */
BYE:
  free_if_non_null(Rcmd);
  free_if_non_null(exec_out);
  return status;
}
int
exists1(
    int sock,
    const char * const val,
    bool *ptr_x
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out = 0;
  char *Rcmd = NULL; 

  if ( sock <  0 ) { go_BYE(-1); } 
  if ( val == NULL ) { go_BYE(-1); } 
  int len = strlen(val) + strlen("exists(\"\")") + 16;
  Rcmd = malloc(len); return_if_malloc_failed(Rcmd);
  memset(Rcmd, 0, len);
  sprintf(Rcmd, "exists(\"%s\")", val);
  status = exec_str(sock, Rcmd, &exec_out, &len_out, XT_ARRAY_BOOL); 
  if ( exec_out == NULL ) { go_BYE(-1); }
  if ( len_out != 8 ) { go_BYE(-1); }
  int num_elements = ((int8_t *)exec_out)[0];
  if ( num_elements != 1 ) { go_BYE(-1); }
  *ptr_x = ((uint32_t *)exec_out)[1] & 0x1;
BYE:
  free_if_non_null(Rcmd);
  free_if_non_null(exec_out);
  return status;
}
    /* When we get back a single LOGICAL, 
     * When we have a FALSE, we get
p out_buf [0] $1 = 1 '\001'
p out_buf [1] $2 = 0 '\000'
p out_buf [2] $3 = 0 '\000'
p out_buf [3] $4 = 0 '\000'
p out_buf [4] $5 = 0 '\000'
p out_buf [5] $6 = -1 '\377'
p out_buf [6] $7 = -1 '\377'
p out_buf [7] $8 = -1 '\377'
     *
     * When we have a TRUE, we get
p out_buf[0] $1 = 1 '\001'
p out_buf[1] $2 = 0 '\000'
p out_buf[2] $3 = 0 '\000'
p out_buf[3] $4 = 0 '\000'
p out_buf[4] $5 = 1 '\001'
p out_buf[5] $6 = -1 '\377'
p out_buf[6] $7 = -1 '\377'
p out_buf[7] $8 = -1 '\377'
     * */

int
is_vector(
    int sock, // INPUT 
    const char * const name, // INPUT 
    bool *ptr_x
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out = 0;
  char *Rcmd = NULL; 

  if ( sock <  0 ) { go_BYE(-1); } 
  if ( name == NULL ) { go_BYE(-1); } 
  int len = strlen(name) + strlen("is.vector(\"\")") + 16;
  Rcmd = malloc(len); return_if_malloc_failed(Rcmd);
  memset(Rcmd, 0, len);
  sprintf(Rcmd, "is.vector(\"%s\")", name);
  status = exec_str(sock, Rcmd, &exec_out, &len_out, XT_ARRAY_BOOL); 
  if ( ( exec_out == NULL ) || ( len_out == 0 ) ) { go_BYE(-1); }
  // TODO: Following needs more thought
  *ptr_x  = exec_out[4]; // explanation below. 
  /* Header response for false and true is shown below 
      01 00 00 00 00 ff ff ff
      01 00 00 00 01 ff ff ff
    */
BYE:
  free_if_non_null(Rcmd);
  free_if_non_null(exec_out);
  return status;
}
