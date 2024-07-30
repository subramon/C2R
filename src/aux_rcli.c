// We use error code of -2 to indicate read() returned 0 
// We use error code of -3 to indicate read() returned < 0 
#include "incs.h"
#include "aux_rcli.h"
#include <errno.h>

//-- TODO How do we interpret the response header?
int 
read_hdr(
    int sock, 
    struct phdr *ph
    )
{
  int status = 0;
  int num_read;
  status = get_buf_from_sock(sock, (char *)ph, HDR_LEN, &num_read); 
  cBYE(status); 
  if ( num_read != HDR_LEN ) { WHEREAMI; } // TODO Assert?
  if ( ph->res != 0 ) { WHEREAMI; }
BYE:
  return status;
}
int
get_buf_from_sock(
    int sock,
    char *buf,
    int buflen,
    int *ptr_num_read
    )
{
  int status = 0;
  if ( sock < 0 ) { go_BYE(-1); }
  if ( buflen < 0 ) { go_BYE(-1); }
  if ( buf == NULL ) { go_BYE(-1); }
  if ( buflen == 0 ) { WHEREAMI; return status; } // Nothing to do 

  int num_to_read = buflen;
  int total_num_read = 0;
  for ( ; ; ) { 
    int num_read = read(sock, buf, buflen);
    if ( num_read == 0 ) { return -2; } 
    /* zero indicates end of file. See 
     * https://man7.org/linux/man-pages/man2/read.2.html */
    if ( num_read < 0 ) { go_BYE(-3); } // server error 
    total_num_read += num_read;
    if ( total_num_read >= num_to_read ) { break; } // all done 
    buf    += num_read;
    buflen -= num_read;
  }
  if ( total_num_read != num_to_read ) { // TODO Assert?
    printf("WARNING! total_num_read, num_to_read = %d, %d \n", 
    total_num_read, num_to_read);
  }
  *ptr_num_read = total_num_read;
BYE:
  return status;
}
//-------------------------------------------------------------
int
mk_dt_string(
    const char * const name, 
    char **ptr_param, 
    size_t *ptr_len_param
    )
{
  int status = 0;
  char *param = NULL; size_t len_param = 0;
  if ( name == NULL ) { go_BYE(-1); }
  // create param 
  size_t len_name = strlen(name) + 1 /* for nullc */;
  // extend len_name so that 4 + len_name is divisible by 8 
  for ( ; ; ) {  // this is a bit of a dumb way to do it 
    if ( ( ( ( len_name + 4 ) / 8 ) * 8 ) != len_name ) {
      len_name++;
    }
    else {
      break;
    }
  }


  len_param = sizeof(int32_t) /* for hdr */ + len_name;
  /* make sure len_param is divisible by 4  
   * See "requirement to pad strings with zeros so the length of the 
   * parameter/content is divisible by 4:. 
   * in  http://rforge.net/Rserve/dev.html */

  status = posix_memalign((void **)&param, 16,  len_param); cBYE(status);
  return_if_malloc_failed(param);
  memset(param, 0, len_param);
  int32_t *iptr = (int32_t *)param;
  iptr[0] = (len_name << 8) | DT_STRING;
  /* Above based on  following macros defined in Rsrv.h
#define PAR_TYPE(X) ((X) & 255)
#define PAR_LEN(X) (((unsigned int)(X)) >> 8)
*/
  strcpy(param+sizeof(int32_t), name);
  *ptr_param = param;
  *ptr_len_param = len_param;
  if ( ( len_param & 3 ) != 0 ) { go_BYE(-1); } // divisible by 4
BYE:
  return status;
}
//-------------------------------------------------------------
int
mk_hdr(
    const char * const command,
    int payload_len, 
    struct phdr *ph
    )
{
  int status = 0;
  if ( command == NULL ) { go_BYE(-1); }
  if ( ph     == NULL ) { go_BYE(-1); }
  if ( payload_len  < 0 ) { go_BYE(-1); }
  memset(ph, 0, HDR_LEN);
  /* memset achieves the following:
  ph->msg_id = 0; 
  message id (since 1.8) [WAS:data offset behind header (ergo usually 0)] 
  ph->res = 0; // not used because our payloads are small enough
  */
  ph->len = payload_len; // length of the packet minus header (ergo -16) 

  if ( strcmp(command, "CMD_shutdown") == 0 ) { 
    ph->cmd = CMD_shutdown;
  }
  else if ( strcmp(command, "CMD_setSEXP") == 0 ) { 
    ph->cmd = CMD_setSEXP;
  }
  else if ( strcmp(command, "CMD_eval") == 0 ) { 
    ph->cmd = CMD_eval;
  }
  else if ( strcmp(command, "CMD_voidEval") == 0 ) { 
    ph->cmd = CMD_voidEval;
  }
  else {
    go_BYE(-1); // Will add more options as they become needed
  }
BYE:
  return status;
}
//-------------------------------------------------------------
int 
close_conn(
    int sock
    )
{
  int status = 0;
  if ( sock < 0 ) { go_BYE(-1); }
  // OLD shutdown(sock, 0);
  // See note at end as to whether to use shutdown() or close()
  status = close(sock); 
  if ( status == -1 ) { 
    fprintf(stderr, "%s:%s:%d = %s \n",
        __FUNCTION__, __FILE__, __LINE__, strerror(errno));
  }

BYE:
  return status;
}
//----------------------------------------------------------------
int
exec_str(
    int sock,
    const char * const str,
    char **ptr_output,
    size_t *ptr_len_out,
    int expected_xt
    )
{
  int status = 0;
  struct phdr ph;
  char hdr [HDR_LEN];
  char *out_buf  = NULL; size_t sz_out_buf = 0;
  char *param1   = NULL; size_t len_param1 = 0; 
  size_t payload_len = 0; int num_read;
  const char *r_cmd = NULL;
  //-----------------------------------------
  if ( sock <  0 ) { go_BYE(-1); }
  if ( str == NULL ) { go_BYE(-1); } 
  status = mk_dt_string(str, &param1, &len_param1);  cBYE(status);
  payload_len = len_param1; 
  // send header 
  if ( ptr_output == NULL ) { 
    r_cmd = "CMD_voidEval";
  }
  else {
    r_cmd = "CMD_eval";
  }
  // printf("r_cmd = %s \n", r_cmd);
  status = mk_hdr(r_cmd, payload_len, &ph); cBYE(status);
  size_t num_sent = send(sock, &ph, HDR_LEN, 0);
  if ( num_sent != HDR_LEN ) { go_BYE(-1); }
  //----------------------
  // send param1
  num_sent = send(sock, param1, len_param1, 0);
  if ( num_sent != len_param1 ) { go_BYE(-1); }
  free_if_non_null(param1);
  //-- first get the header 
  status = read_hdr(sock, &ph); cBYE(status);
  payload_len = ph.len; 
  // Why did I have to use ph.res?
  // Because I was not draining the socket completely in previous call!!
  //-------------------------
  //-- now get the payload 
  if ( ptr_output != NULL ) { 
    *ptr_output  = NULL;
    *ptr_len_out = 0;
    if ( payload_len == 0 ) { go_BYE(-1); }
    // I USED TO HAVE: if ( payload_len == 0 ) { WHEREAMI; goto BYE; }
    sz_out_buf = payload_len - 8; // The 8 is explained below 
    out_buf = malloc(sz_out_buf);
    return_if_malloc_failed(out_buf);
    memset(out_buf, 0, sz_out_buf);

    // Next 8 bytes explained as follows
    // First 4 bytes indicate dt_val e.g., DT_SEXP
    // Next  4 bytes indicate xt_val e.g.,  XT_ARRAY_STR
    // Note that ARRAY_STR is for this example, we may want to generalize
    memset(hdr, 0, HDR_LEN);
    status = get_buf_from_sock(sock, hdr, 8, &num_read);  cBYE(status);
    int dt_val = ((uint32_t *)hdr)[0] & 0xFF;
    if ( dt_val != DT_SEXP ) { go_BYE(-1); }
    if ( expected_xt >= 0 ) { 
      int xt_val = ((uint32_t *)hdr)[1] & 0xFF;
      if ( xt_val != expected_xt ) { go_BYE(-1); }
    }
    status = get_buf_from_sock(sock, (char *)out_buf, sz_out_buf, &num_read);
    cBYE(status);
    *ptr_output = out_buf;
    *ptr_len_out = sz_out_buf; // TODO P2 Should this be num_read? 
  }
BYE:
  // Don't do free_if_non_null(out_buf); Free done by caller 
  if ( ( str != NULL ) && ( status != 0 ) )  {
    printf("Failed to execute [%s]\n", str);
  }
  if ( status < 0 ) { 
    free_if_non_null(out_buf);
  }
  free_if_non_null(param1);
  return status;
}
//----------------------------------------------------------------
// shutdown the server
int
rshutdown(
    int sock
    )
{
  int status = 0;
  char outbuf[32]; 
  struct phdr ph;

  status = mk_hdr("CMD_shutdown", 0, &ph); cBYE(status);
  size_t num_sent = send(sock, &ph, HDR_LEN, 0);
  if ( num_sent != HDR_LEN ) { go_BYE(-1); }
  ssize_t nr = read(sock, outbuf, 32);
  if ( nr == 0 ) { return -2; } 
  if ( nr  < 0 ) { return -3; } 
  if ( ( nr != 16 ) && ( nr != 32 ) ) { 
    fprintf(stderr, "ERROR: On shutdown, Received %lu \n", nr); 
  }
  status = close_conn(sock); cBYE(status);
BYE:
  return status;
}
//----------------------------------------------------------------
int
rconnect(
    const char * const server, 
    int portnum, 
    int snd_timeout_sec,
    int rcv_timeout_sec,
    int *ptr_sock
    )
{
  int status = 0;
  int sock = 0;
  char hdr[32]; int num_read;
  struct sockaddr_in serv_addr; 
  memset(&serv_addr, 0, sizeof(struct sockaddr_in));

  if ( server == NULL ) { go_BYE(-1); } 
  if ( portnum <= 0 ) { go_BYE(-1); } 
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if ( sock < 0 ) { 
    printf("Socket creation error  %s %d\n", __FILE__, __LINE__); 
    status = -1; goto BYE;
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portnum);
  // Convert IPv4 and IPv6 addresses from text to binary form
  status = inet_pton(AF_INET, server, &serv_addr.sin_addr); cBYE(status);

  // Establish connection to socket
  status = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  cBYE(status);
#define CAN_TCP_NODELAY
#ifdef CAN_TCP_NODELAY
  int flag = 1;
  int result = setsockopt(sock,            /* socket affected */
      IPPROTO_TCP,     /* set option at TCP level */
      TCP_NODELAY,     /* name of option */
      (char *) &flag,  /* the cast is historical cruft */
      sizeof(int));    /* length of option value */
  if ( result < 0 )  { go_BYE(-1); }
#endif
  // set timeouts for receiving
  if ( rcv_timeout_sec > 0 ) { 
    struct timeval timeout = { .tv_sec = rcv_timeout_sec };
    status = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                sizeof(timeout));
    cBYE(status);
  }
  // set timeouts for sending 
  if ( snd_timeout_sec > 0 ) { 
    struct timeval timeout = { .tv_sec = snd_timeout_sec };
    status = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                sizeof(timeout));
    cBYE(status);
  }
  memset(hdr, 0, 32);
  status = get_buf_from_sock(sock, hdr, 32, &num_read); cBYE(status);
  /* After connection is established, the server sends 32 bytes 
   * representing the ID-string defining the capabilities of the server. 
   * */
  if ( num_read != 32 ) { go_BYE(-1); }
  if ( strncmp(hdr, "Rsrv0103QAP1", strlen("Rsrv0103QAP1")) != 0 ) {
    go_BYE(-1);
  }
  *ptr_sock = sock;
BYE:
  if ( status < 0 ) { 
    fprintf(stderr, "Unable to connect to [%s] on %d \n", server, portnum);
  }
  return status;
}


/*
 * https://www.baeldung.com/cs/sockets-close-vs-shutdown
 * The close operation blocks the communication of a socket and destroys it. In other words, closing a socket represents terminating its connections (if there are any) and then close the file descriptor that enables the message exchanging. In this way, after executing a close operation, the process can neither read either write the socket. Thus, any attempt to operate on a closed socket will raise an exception to the process:
 *
 * The shutdown operation represents a way to block the communication of a socket without destroying it. But, besides calling a socket shutdown, it is necessary to define how this operation will act. To do that, three execution modes are available: SHUT_RD, SHUT_WR, and SHUT_RDWR. These execution modes are described next.
 * */
