#include "incs.h"
#include "aux_rcli.h"
#include "set_vec.h"
#include "chk_R_class.h"
#include "n_df.h"
#include "get_bool.h"
#include "get_named_vec.h"
#include "get_F8.h"
#include "get_I4.h"
#include "exists.h"
#include "get_vec.h"

//---------------------------------------------------------
// Purpose: Send and receive an F8 vector of length n. Repeat niters
static int
test1(
    int sock,
    int n,
    int niters
    )
{
  int status = 0;
  char *xF8 = NULL, *chk_xF8 = NULL; int chk_n;
  // make some data to send 
  xF8 = malloc(n *sizeof(double));
  for ( int i = 0; i < n; i++ ) { 
    xF8[i] = 10*(i+1);
  }
  //------
  for ( int i = 0; i < niters; i++ ) {
    // send some data 
    status = set_vec(sock, "vecF8", "F8", xF8, n, 0); cBYE(status); 
    // get some data 
    free_if_non_null(chk_xF8);
    status = get_vec(sock, "vecF8", "F8", &chk_xF8, &chk_n); cBYE(status);
    // check data sent same as data returned
    if ( n != chk_n ) { go_BYE(-1); }
    if ( chk_xF8 == NULL ) { go_BYE(-1); }
    for ( int j = 0; j < n; j++ ) {
      if ( ((double *)xF8)[j] != ((double *)chk_xF8)[j] ) { go_BYE(-1); }
    }
  }
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(xF8);
  free_if_non_null(chk_xF8);
  return status;
}
//---------------------------------------------------------
// This test 
// (1) defines a function "f" in R
// (1) checks that class of f is "function"
// (2) invokes the function
// (3) Checks that the result is as expcted
// Assumption is that the function returns a string 
static int 
test2(
    int sock,
    int niters
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out;
  const char * const fn_define = 
    "f <- function(n) { print(paste('hello world', n+1)) }";
  const char * const fn_execute = "f(10)";
  const char * const fn_class_in = "class(f)";
  const char * const fn_result = "hello world 11";


  for ( int i = 0; i < niters; i++ ) { 
    // create a bogus function f1
    status = exec_str(sock, fn_define, NULL, NULL, -1); cBYE(status);
    // check that function got created
    free_if_non_null(exec_out); len_out = 0;
    status = exec_str(sock, fn_class_in,  &exec_out, &len_out, XT_ARRAY_STR); 
    if ( exec_out == NULL ) { go_BYE(-1); }
    if ( strcmp(exec_out, "function") != 0 ) { go_BYE(-1); }
    if ( !chk_R_class(sock, "f", "function") ) { go_BYE(-1); }

    // execute the function 
    free_if_non_null(exec_out); len_out = 0;
    status = exec_str(sock, fn_execute, &exec_out, &len_out, XT_ARRAY_STR); 
    cBYE(status);
    if ( len_out != 16 ) { go_BYE(-1); }
    if ( exec_out == NULL ) { go_BYE(-1); }
    if ( strcmp(exec_out, fn_result) != 0 )  { go_BYE(-1); }
  }
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(exec_out);
  return status;
}
// Tests creation of vector of type SC
static int 
test3_SC(
    int sock,
    int n,
    int niters
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out; int chk_n;
  char *data = NULL; uint32_t width = 16;
  // make some data to send 
  // [ "a", "ab", "abc", ....] 
  data = malloc(width * n); memset(data, 0, width*n);
  uint32_t len = 1;
  uint32_t cum_len = 0;
  for ( int i = 0; i < n; i++ ) { 
    char buf[width]; memset(buf, 0, width);
    for ( uint32_t j = 0; j < len; j++ ) { 
      buf[j] = 'a' + j;
    }
    memcpy(data+(i*width), buf, strlen(buf));
    cum_len += (len+1); 
    len++;
    if ( len >= width ) { len = 1; }
  }

  // create vector  xSC
  for ( int i = 0; i < niters; i++ ) { 
    status = set_vec(sock, "vecSC", "SC", data, n, width); cBYE(status); 
    free_if_non_null(exec_out); len_out = 0;
    status = exec_str(sock, "class(vecSC)", &exec_out, 
        &len_out, XT_ARRAY_STR); 
    if ( exec_out == NULL ) { go_BYE(-1); }
    if ( strcmp(exec_out, "character") != 0 ) { go_BYE(-1); } 
    if ( !chk_R_class(sock, "vecSC", "character") ) { go_BYE(-1); } 
    status = get_vec_len(sock, "vecSC", &chk_n);
    if ( chk_n != (int)n ) { go_BYE(-1); } 
  }
  //-------------------
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(exec_out);
  free_if_non_null(data);
  return status;
}
//---------------------------------------------------------
// Tests creation of dataframe where individual vectors are 
// of type F8, I4, I1
static int 
test3(
    int sock,
    int n,
    int niters
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out; int chk_n;
  double  *xF8 = NULL, *chk_xF8 = NULL; 
  int32_t *xI4 = NULL, *chk_xI4 = NULL; 
  int8_t  *xI1 = NULL, *chk_xI1 = NULL; 
  // int chk_n;
  // make some data to send 
  xF8 = malloc(n *sizeof(double));
  for ( int i = 0; i < n; i++ ) { xF8[i] = 10*(i+1); }

  xI4 = malloc(n *sizeof(int32_t));
  int32_t j = 0;
  for ( int i = 0; i < n; i++, j++ ) { 
    xI4[i] = j; if ( j == 16 ) {j = 0;} 
  }

  xI1 = malloc(n *sizeof(int8_t));
  j = 0;
  for ( int i = 0; i < n; i++ ) { 
    xI1[i] = j; 
    if ( j == 1 ) { j = 0; continue; } 
    if ( j == 0 ) { j = 1; continue; } 
    go_BYE(-1); 
  } 
  const char * const mk_df = "df <- data.frame(vecF8, vecI4, vecI1) ";

  // create vector  xF8
  status = set_vec(sock, "vecF8", "F8", xF8, n, 0); cBYE(status); 
  free_if_non_null(exec_out); len_out = 0;
  status = exec_str(sock, "class(vecF8)", &exec_out, &len_out, XT_ARRAY_STR); 
  if ( exec_out == NULL ) { go_BYE(-1); }
  if ( strcmp(exec_out, "numeric") != 0 ) { go_BYE(-1); } 
  if ( !chk_R_class(sock, "vecF8", "numeric") ) { go_BYE(-1); } 
  status = get_vec_len(sock, "vecF8", &chk_n);
  if ( chk_n != n ) { go_BYE(-1); } 
  // create vector  xI4
  status = set_vec(sock, "vecI4", "I4", xI4, n, 0); cBYE(status); 
  free_if_non_null(exec_out); len_out = 0;
  status = exec_str(sock, "class(vecI4)", &exec_out, &len_out, XT_ARRAY_STR); 
  if ( exec_out == NULL ) { go_BYE(-1); }
  if ( strcmp(exec_out, "integer") != 0 ) { go_BYE(-1); } 
  if ( !chk_R_class(sock, "vecI4", "integer") ) { go_BYE(-1); } 
  status = get_vec_len(sock, "vecI4", &chk_n);
  if ( chk_n != n ) { go_BYE(-1); } 
  // create vector  xI4
  // create vector  xI1
  status = set_vec(sock, "vecI1", "I1", xI1, n, 0); cBYE(status); 
  free_if_non_null(exec_out); len_out = 0;
  status = exec_str(sock, "class(vecI1)", &exec_out, &len_out, XT_ARRAY_STR); 
  if ( exec_out == NULL ) { go_BYE(-1); }
  if ( strcmp(exec_out, "logical") != 0 ) { go_BYE(-1); } 
  if ( !chk_R_class(sock, "vecI1", "logical") ) { go_BYE(-1); }
  status = get_vec_len(sock, "vecI1", &chk_n);
  if ( chk_n != n ) { go_BYE(-1); } 
  //-----------------------------------------------
  for ( int i = 0; i < niters; i++ ) { 
    // make a dataframe using xF8, XI4, XI1
    status = exec_str(sock, mk_df, NULL, NULL, -1); cBYE(status);
    // check that dataframe got created
    free_if_non_null(exec_out); len_out = 0;
    status = exec_str(sock, "class(df)", &exec_out, &len_out, XT_ARRAY_STR); 
    if ( len_out != 12 ) { go_BYE(-1); }
    if ( exec_out == NULL ) { go_BYE(-1); }
    if ( strcmp(exec_out, "data.frame") != 0 )  { go_BYE(-1); }
    // access the columns of the data frame 
    if ( !chk_R_class(sock, "df", "data.frame") ) { go_BYE(-1); }
    if ( ncols_df(sock, "df") != 3 ) { go_BYE(-1); } 
    // check contents of vecI4
    status = exec_str(sock, "chk_vecI4 = df$vecI4",  NULL, NULL, -1);
    status = get_vec(sock, "chk_vecI4", "I4", (char **)&chk_xI4, &chk_n); 
    cBYE(status);
    if ( n != chk_n ) { go_BYE(-1); }
    if ( chk_xI4 == NULL ) { go_BYE(-1); }
    for ( int k = 0; k < n; k++ ) {
      if ( ((int32_t *)xI4)[k] != ((int32_t *)chk_xI4)[k] ) { go_BYE(-1); }
    }
    free_if_non_null(chk_xI4);
    // check contents of vecF8
    status = exec_str(sock, "chk_vecF8 = df$vecF8",  NULL, NULL, -1);
    status = get_vec(sock, "chk_vecF8", "F8", (char **)&chk_xF8, &chk_n); 
    cBYE(status);
    if ( n != chk_n ) { go_BYE(-1); }
    if ( chk_xF8 == NULL ) { go_BYE(-1); }
    for ( int k = 0; k < n; k++ ) {
      if ( ((double *)xF8)[k] != ((double *)chk_xF8)[k] ) { go_BYE(-1); }
    }
    free_if_non_null(chk_xF8);
    // check contents of vecI1
    status = exec_str(sock, "chk_vecI1 = df$vecI1",  NULL, NULL, -1);
    status = get_vec(sock, "chk_vecI1", "I1", (char **)&chk_xI1, &chk_n); 
    cBYE(status);
    // check data sent same as data returned
    if ( n != chk_n ) { go_BYE(-1); }
    if ( chk_xI1 == NULL ) { go_BYE(-1); }
    for ( int k = 0; k < n; k++ ) {
      if ( ((int8_t *)xI1)[k] != ((int8_t *)chk_xI1)[k] ) { go_BYE(-1); }
    }
    free_if_non_null(chk_xI1);
    //--------------------------------------------------
  }
  //-------------------
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(exec_out);
  free_if_non_null(xF8); free_if_non_null(chk_xF8);
  free_if_non_null(xI4); free_if_non_null(chk_xI4);
  free_if_non_null(xI1); free_if_non_null(chk_xI1);
  return status;
}
//---------------------------------------------------------
// Tests how to set a bool on the R side and retrieve a bool from 
static int 
test4(
    int sock,
    int niters
    )
{
  int status = 0;
  for ( int i = 0; i < niters; i++ ) { 
    bool x, y;
    if (  ( i % 2 ) == 0 ) { x = true; } else { x = false; }
    status = set_bool(sock, "x", x); cBYE(status);
    status = get_bool(sock, "x", &y); cBYE(status);
    if ( x != y ) { go_BYE(-1); }
  }
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  return status;
}
//---------------------------------------------------------
// Tests how exists(...) works versus boolean
static int 
test5(
    int sock,
    int niters
    )
{
  int status = 0;
  int n = 1024; double *xF8 = NULL;
  // make a vector called "some_vec"
  xF8 = malloc(n *sizeof(double));
  for ( int i = 0; i < n; i++ ) { xF8[i] = 10*(i+1); }
  status = set_vec(sock, "some_vec", "F8", xF8, n, 0); cBYE(status); 
  //------
  for ( int i = 0; i < niters; i++ ) {
    bool x;
    status = exists1(sock, "some_vec", &x); cBYE(status);
    if ( x != true ) { go_BYE(-1); }
    status = exists2(sock, "some_vec", &x); cBYE(status);
    if ( x != true ) { go_BYE(-1); }
  }
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  return status;
}
//---------------------------------------------------------
// Test creation of a formula 
static int
test6(
    int sock,
    int niters
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out = 0;
  const char * const str_frmla = 
    "frmla = \"demand ~ s(xxx, bs = 're') + offset(yyy) + "
    " day1 + day2 + s(zzz, bs = 'cc', k = 25) + "
    " s(www, by = n_ind) \"";

  for ( int i = 0; i < niters; i++ ) { 
    // define mgcv library 
    status = exec_str(sock, "library(mgcv)", NULL, NULL, -1); cBYE(status);
    // define formula 
    status = exec_str(sock, str_frmla, NULL, NULL, -1); cBYE(status);
    // check that str_formula got created 
    free_if_non_null(exec_out); len_out = 0;
    status = exec_str(sock, "frmla", &exec_out, &len_out, XT_ARRAY_STR); 
    if ( exec_out == NULL ) { go_BYE(-1); }
    // compile the formula 
    status = exec_str(sock, "compiled_frmla = as.formula(frmla)", 
      NULL, NULL, -1); 
    cBYE(status);
    // check type of compiled_frmla
    free_if_non_null(exec_out); len_out = 0;
    status = exec_str(sock, "class(compiled_frmla)", &exec_out, 
        &len_out, XT_ARRAY_STR); 
    if ( exec_out == NULL ) { go_BYE(-1); }
    if ( strcmp(exec_out, "formula") != 0 ) { go_BYE(-1); }
    if ( !chk_R_class(sock, "compiled_frmla", "formula") ) { go_BYE(-1); }
  }
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(exec_out);
  return status;
}
//---------------------------------------------------------
// Test creation of a named vector
static int
test7(
    int sock,
    int niters
    )
{
  int status = 0;
  const char * const str_mk_vec = 
    " x <- c(a = 1.23, bc = 4.56, def = 7.89, ghij = 1111)";
  char key_blob[1024]; int sz_key_blob = sizeof(key_blob); 
  double dvals[4]; int sz_vals = sizeof(dvals); 
  uint32_t n_key_blob, n_vals; 

  for ( int i = 0; i < niters; i++ ) { 
    // make named vector
    status = exec_str(sock, str_mk_vec, NULL, NULL, -1); cBYE(status);
    // retrieve named vector 
    status = get_named_vec_F8(sock, "x", 
        key_blob, sz_key_blob, &n_key_blob, dvals, sz_vals, &n_vals);
    cBYE(status);
    if ( n_vals != 4 ) { go_BYE(-1); } 
    if ( dvals[0] != 1.23 ) { go_BYE(-1); }
    if ( dvals[1] != 4.56 ) { go_BYE(-1); }
    if ( dvals[2] != 7.89 ) { go_BYE(-1); }
    if ( dvals[3] != 1111 ) { go_BYE(-1); }
    if ( n_key_blob != 16 ) { go_BYE(-1); } 
    if ( strcmp(key_blob, "a") != 0 ) { go_BYE(-1); }
    if ( strcmp(key_blob+2, "bc") != 0 ) { go_BYE(-1); }
    if ( strcmp(key_blob+5, "def") != 0 ) { go_BYE(-1); }
    if ( strcmp(key_blob+9, "ghij") != 0 ) { go_BYE(-1); }
  }
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  return status;
}
//---------------------------------------------------------
// Test getting an I4 from R
static int
test8_I4(
    int sock,
    int niters
    )
{
  int status = 0;
  char *exec_out = NULL; size_t len_out = 0;
  //-----------------
  for ( int i = 0; i < niters; i++ ) { 
    int yval;
    status = exec_str(sock, "y = as.integer(123456)", NULL, NULL, -1); cBYE(status);
    status = exec_str(sock, "class(y)", &exec_out, &len_out, -1); cBYE(status);
    if ( strcmp(exec_out, "integer") != 0 ) { go_BYE(-1); }
    free_if_non_null(exec_out);
    status = get_I4(sock, "y", &yval); cBYE(status);
    if ( yval != 123456 ) { go_BYE(-1); } 
  }
  //-------------------------
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(exec_out);
  return status;
}
//---------------------------------------------------------
// Test getting a F8 from R
static int
test8(
    int sock,
    int niters
    )
{
  int status = 0;
  //-----------------
  for ( int i = 0; i < niters; i++ ) { 
    double yval;
    status = exec_str(sock, "y = 123.456", NULL, NULL, -1); cBYE(status);
    status = get_F8(sock, "y", &yval); cBYE(status);
    if ( yval != 123.456 ) { go_BYE(-1); } 
  }
  //-------------------------
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  return status;
}
//---------------------------------------------------------
// Purpose: Get length of vector 
static int
test9(
    int sock,
    int n,
    int niters
    )
{
  int status = 0;
  double *xF8 = NULL;  int chk_n;
  // make some data to send 
  xF8 = malloc(n *sizeof(double));
  for ( int i = 0; i < n; i++ ) { xF8[i] = 0; }
  //------
  for ( int i = 0; i < niters; i++ ) {
    // send some data 
    status = set_vec(sock, "vecF8", "F8", xF8, n, 0); cBYE(status); 
    // get length of vector 
    status = get_vec_len(sock, "vecF8", &chk_n); cBYE(status);
    if ( n != chk_n ) { go_BYE(-1); }
  }
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(xF8);
  return status;
}
//---------------------------------------------------------
// This test 
// (1) checks timeout for sockets
// Assumption is that the function returns a string 
static int 
test10(
    const char * const server,
    int portnum,
    int niters
    )
{
  int status = 0;
  int sock = -1;
  printf("Starting %s \n", __FUNCTION__); 
  // Note that we provide 5 seconds for socket timeout 
  status = rconnect(server, portnum, 5, 5, &sock); cBYE(status);
  char *exec_out = NULL; size_t len_out;
  // In case1, function takes *MORE* time than socket allows
  // In case2, function takes *LESS* time than socket allows
  const char * const fn_case1 = 
    "f <- function(n) {  "
    " sum = 0; for ( x in 1:200000000 ) { sum = sum+x }; return(sum) } ";
  // TODO P2 If I put a sleep in fn_case2, however small, test fails
  const char * const fn_case2 = 
    "f <- function(n) { print(paste('hello world', n+1)) }";
  const char * const fn_execute = "f(10)";
  const char * const fn_class_in = "class(f)";

  for ( int mode = 1; mode <= 2; mode++ ) {
    for ( int i = 0; i < niters; i++ ) { 
      status = rconnect(server, portnum, 5, 5, &sock); cBYE(status);
      printf("Mode = %d \n", mode); 
      status = exec_str(sock, "rm(f)", NULL, NULL, -1); 
      switch ( mode ) { 
        case 1 : status = exec_str(sock, fn_case1, NULL, NULL, -1); break;
        case 2 : status = exec_str(sock, fn_case2, NULL, NULL, -1); break;
        default : go_BYE(-1); break;
      }
      cBYE(status); 
      // check that function got created
      free_if_non_null(exec_out); len_out = 0;
      status = exec_str(sock, fn_class_in,  &exec_out,&len_out,XT_ARRAY_STR); 
      if ( exec_out == NULL ) { go_BYE(-1); }
      if ( strcmp(exec_out, "function") != 0 ) { go_BYE(-1); }
      if ( !chk_R_class(sock, "f", "function") ) { go_BYE(-1); }

      if ( sock <= 0 ) { go_BYE(-1); }
      // execute the function 
      free_if_non_null(exec_out); len_out = 0;
      status = exec_str(sock, fn_execute, &exec_out,&len_out,XT_ARRAY_STR); 
      switch ( mode ) { 
        case 1 : if ( status == 0 ) { go_BYE(-1); } break;
        case 2 : cBYE(status); break;
        default : go_BYE(-1); break;
      }
      status = close_conn(sock); sock = -1; 
    }
  }
  printf("Completed %s \n", __FUNCTION__); 
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  if ( status < 0 ) { 
    printf("Failed %s \n", __FUNCTION__); }
  else {
  printf("Succeeded %s \n", __FUNCTION__); 
  }
  if ( sock > 0 ) { 
    status = close_conn(sock); if ( status < 0 ) { WHEREAMI; }
  }
  free_if_non_null(exec_out);
  return status;
}
//---------------------------------------------------------
// Purpose: check packages loaded
static int
test11(
    int sock,
    int niters
    )
{
  int status = 0;
  bool bval;
  char *exec_out = NULL; size_t len_out;
  const char * const is_pkg1 = 
        "\"mgcv\" %in% tolower(suppressWarnings(library()$results[,1]))";
  const char * const is_pkg2 = 
        "\"bogus\" %in% tolower(suppressWarnings(library()$results[,1]))";

  for ( int i = 0; i < niters; i++ ) {
    status = exec_str(sock, is_pkg1, &exec_out, &len_out, XT_ARRAY_BOOL); 
    if ( ( exec_out == NULL ) || ( len_out == 0 ) ) { go_BYE(-1); }
    bval = exec_out[4];
    // printf("%d:%s:%s\n", i, bval ? "true" : "false", is_pkg1);
    if ( !bval ) { go_BYE(-1); }
    free_if_non_null(exec_out);

    status = exec_str(sock, is_pkg2, &exec_out, &len_out, XT_ARRAY_BOOL); 
    if ( ( exec_out == NULL ) || ( len_out == 0 ) ) { go_BYE(-1); }
    bval = exec_out[4];
    // printf("%d:%s:%s\n", i, bval ? "true" : "false", is_pkg2);
    free_if_non_null(exec_out);
    if ( bval ) { go_BYE(-1); }

#define MGCV_LOADED
#ifdef MGCV_LOADED
    status = exec_str(sock, "class(callgam)", &exec_out, 
        &len_out, XT_ARRAY_STR); 
    if ( ( exec_out == NULL ) || ( len_out == 0 ) ) { go_BYE(-1); }
    if ( strcmp(exec_out, "function") != 0 )  { go_BYE(-1); }
#endif
  }
  printf("Completed %s \n", __FUNCTION__); 
  printf("Successfully completed %s \n", __FUNCTION__);
BYE:
  free_if_non_null(exec_out);
  return status;
}
//---------------------------------------------------------
//---------------------------------------------------------
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  int portnum = 6311;
  int sock = -1;
  const char * const server = "127.0.0.1";

  if ( argc != 3 ) { go_BYE(-1); }
  int n = atoi(argv[1]); // size of vectors to create 
  if ( n <= 0 ) { go_BYE(-1); }
  int niters = atoi(argv[2]); // number of iterations to perform
  if ( niters <= 0 ) { go_BYE(-1); }

  status = rconnect(server, portnum, 0, 0, &sock); cBYE(status);
  printf("Established connection\n");
  // TODO status = test11(sock, niters);  cBYE(status);

  status = test8_I4(sock, niters);  cBYE(status);

  status = test1(sock, n, niters);  cBYE(status);
  status = test2(sock, niters);  cBYE(status);
  status = test3(sock, n, niters);  cBYE(status);
  status = test3_SC(sock, n, niters);  cBYE(status);
  status = test4(sock, niters);  cBYE(status);
  status = test5(sock, niters);  cBYE(status);
  status = test6(sock, niters);  cBYE(status);
  status = test7(sock, niters);  cBYE(status);
  status = test8(sock, niters);  cBYE(status);
  status = test9(sock, n, niters);  cBYE(status);
  status = test10(server, portnum, niters);  cBYE(status);
  printf("Test %s completed successfully\n", argv[0]);
BYE:
  if ( sock > 0 ) { 
    status = close_conn(sock); if ( status < 0 ) { WHEREAMI; }
    printf("Closed connection\n");
  }
  return status;
}
