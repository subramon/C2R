extern int
get_vec(
    int sock,
    const char * const name,
    const char * const qtype,
    char ** restrict ptr_data, // [n]
    int * restrict ptr_n
    );
