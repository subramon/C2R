#include "incs.h"
#include "macros.h"
#include "mk_df_str.h"
char *
mk_df_str(
    char **col_names, // [ncols][]
    char **col_types, // [ncols][]
    int ncols
    )
{
  char *buf = NULL;
  if ( col_names == NULL ) { go_BYE(-1); } 
  if ( col_types == NULL ) { go_BYE(-1); } 
  if ( ncols < 1 ) { go_BYE(-1); } 

  int len = strlen("rsdf <- data.frame(") + 32;
  for ( int i = 0; i < ncols; i++ ) { 
    if ( col_types[i] == NULL ) { go_BYE(-1); } 
    if ( col_types != NULL ) { 
      if ( strcmp(col_types[i], "XX") == 0 ) { continue; }
    }
    if ( col_names[i] == NULL ) { go_BYE(-1); } 
    len += ( strlen(col_names[i]) + 4);
  }
  buf = malloc(len);
  memset(buf, 0, len);
  strcpy(buf, "rsdf <- data.frame(");
  for ( int i = 0; i < ncols; i++ ) { 
    if ( i > 0 ) { 
      strcat(buf, ", ");
    }
    strcat(buf, col_names[i]);
  }
  strcat(buf, ")");

  return buf;
}
