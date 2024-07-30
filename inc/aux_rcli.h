#include "r_constants.h"
extern int 
read_hdr(
    int sock, 
    struct phdr *ph
    );
extern int
get_buf_from_sock(
    int sock,
    char *buf,
    int buflen,
    int *ptr_num_read
    );
extern int
mk_dt_string(
    const char * const name, 
    char **ptr_param, 
    size_t *ptr_len_param
    );
extern int
mk_hdr(
    const char * const command,
    int payload_len, 
    struct phdr *ph
    );
extern int 
close_conn(
    int sock
    );
extern int
exec_str(
    int sock,
    const char * const str,
    char **ptr_output,
    size_t *ptr_len_out,
    int expected_xt
    );
extern int
rshutdown(
    int sock
    );
extern int
rconnect(
    const char * const server, 
    int portnum, 
    int snd_timeout_sec,
    int rcv_timeout_sec,
    int *ptr_sock
    );
