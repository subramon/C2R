extern int
set_vec(
    int sock,
    const char * const name,
    const char * const qtype,
    void *data, // [n]
    uint32_t n,
    uint32_t width
    );
