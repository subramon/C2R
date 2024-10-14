extern int
get_vec(
    int sock,
    const char * const name,
    const char * const qtype,
    char ** restrict ptr_data, // [n]
    size_t * restrict ptr_n
    );
extern int
get_vec_len(
    int sock, // INPUT 
    const char * const name, // INPUT 
    size_t *ptr_n
    );
