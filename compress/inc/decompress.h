extern int 
decompress(
    char * pkt, 
    int *ptr_n,
    int *ptr_m,
    Rtype_t **ptr_Rtypes, // [m]
    size_t **ptr_offsets, // [m]
    char **ptr_out_data,
    char **ptr_out_names
    );
