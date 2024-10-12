#include "incs.h"
#include "str_array_R_to_C.h"
int
str_array_R_to_C(
    const char * Rstr,
    size_t Rsz,
    uint32_t width,
    uint32_t n,
    char ***ptr_Cstr
    )
{
  int status = 0;
  char **Cstr = NULL;
  *ptr_Cstr = NULL; 

  if ( width == 0 ) { go_BYE(-1); } 
  if ( n == 0 ) { go_BYE(-1); } 
  // Check that n is consistent with Rinformation
  uint32_t chk_n = 0;
  for ( uint32_t i = 0; i < Rsz; i++ ) { 
    if ( Rstr[i] == '\0' ) { chk_n++; }
  }
  if ( chk_n != n ) { go_BYE(-1); }
  // Check that width is consistent with Rinformation
  const char *cptr = Rstr;
  for ( uint32_t i = 0; i < n; i++ ) { 
    if ( strlen(cptr) >= n ) { go_BYE(-1); } 
    cptr += strlen(cptr); 
  }
  // allocate Cstr
  Cstr = malloc(n * sizeof(char *));
  for ( uint32_t i = 0; i < n; i++ ) { 
    Cstr[i] = malloc(width);
    memset(Cstr[i], 0,  width);
  }
  // copy individual strings 
  cptr = Rstr;
  for ( uint32_t i = 0; i < n; i++ ) { 
    strcpy(Cstr[i], cptr); 
    cptr += strlen(cptr) + 1;
  }
  *ptr_Cstr = Cstr;
BYE:
  return status;
}
