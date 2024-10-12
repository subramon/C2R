#include "incs.h"
#include "str_array_R_to_C.h"
int
str_array_R_to_C(
    const char * Rstr,
    size_t Rsz,
    uint32_t width,
    uint32_t n,
    char **ptr_Cstr,
    char ***ptr_Carr
    )
{
  int status = 0;
  char **Carr = NULL; char *Cstr = NULL; 
  

  if ( ( ptr_Carr != NULL ) && ( ptr_Cstr != NULL ) ) { go_BYE(-1); }
  if ( ( ptr_Carr == NULL ) && ( ptr_Cstr == NULL ) ) { go_BYE(-1); }

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
  // return either Cstr or Carr
  if ( ptr_Carr != NULL ) { 
    // allocate Carr
    Carr = malloc(n * sizeof(char *));
    for ( uint32_t i = 0; i < n; i++ ) { 
      Carr[i] = malloc(width);
      memset(Carr[i], 0,  width);
    }
    // copy individual strings 
    cptr = Rstr;
    for ( uint32_t i = 0; i < n; i++ ) { 
      strcpy(Carr[i], cptr); 
      cptr += strlen(cptr) + 1;
    }
    *ptr_Carr = Carr;
  }
  if ( ptr_Cstr != NULL ) { 
    // allocate Cstr
    Cstr = malloc(n * width);
    memset(Cstr, 0,  n * width);
    // copy individual strings 
    cptr = Rstr;
    char *dst = Cstr; 
    for ( uint32_t i = 0; i < n; i++ ) { 
      strcpy(dst, cptr); 
      cptr += strlen(cptr) + 1;
      dst += width; 
    }
    *ptr_Cstr = Cstr;
  }
BYE:
  return status;
}
