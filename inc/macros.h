#ifndef __Q_MACROS_H
#define __Q_MACROS_H
#define WHEREAMI { fprintf(stderr, "Line %3d of File %s \n", __LINE__, __FILE__);  }
#define go_BYE(x) { WHEREAMI; status = x ; goto BYE; }
#define cBYE(x) { if ( (x) < 0 ) { go_BYE((x)) } }
#define fclose_if_non_null(x) { if ( (x) != NULL ) { fclose((x)); (x) = NULL; } } 
#define free_if_non_null(x) { if ( (x) != NULL ) { free((x)); (x) = NULL; } }
#define return_if_fopen_failed(fp, file_name, access_mode) { if ( fp == NULL ) { fprintf(stderr, "Unable to open file %s for %s \n", file_name, access_mode); go_BYE(-1); } }
#define return_if_malloc_failed(x) { if ( x == NULL ) { fprintf(stderr, "Unable to allocate memory\n"); go_BYE(-1); } }

#define return_if_opendir_failed(dr, root_dir) { \
  if ( dr == NULL)  { \
    fprintf(stderr, "Could not open directory [%s]\n", root_dir ); \
    go_BYE(-1); \
  } \
}
#define mcr_min(X, Y)  ((X) < (Y) ? (X) : (Y))

#define mcr_rs_munmap(X, nX) { \
  if ( ( X == NULL ) && ( nX != 0 ) ) {  WHEREAMI; } \
  if ( ( X != NULL ) && ( nX == 0 ) )  { WHEREAMI; } \
  if ( X != NULL ) { \
  	int l_status = munmap(X, nX); if ( l_status != 0 ) { WHEREAMI; } \
  } \
  X = NULL; nX = 0;  \
}

#endif
