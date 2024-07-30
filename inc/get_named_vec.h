extern int
get_named_vec_F8(
    int sock,
    const char * const name,

    char * restrict key_blob, // [sz_key_blob] 
    uint32_t sz_key_blob,
    uint32_t *ptr_n_key_blob, // 0 < *ptr_n_key_blob <= sz_key_blob

    double * restrict vals, // [sz_vals] 
    uint32_t sz_vals, 
    uint32_t *ptr_n_vals   // 0 < *ptr_n_vals <= sz_vals
    );
