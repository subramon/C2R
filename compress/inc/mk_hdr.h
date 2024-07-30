#include "qtypes.h"
#include "compress_struct.h"
extern char *
mk_hdr(
    int n,
    int m1,
    int m2,
    const qtype_t * const qtypes, // [m1]
    const Rtype_t * const Rtypes, // [m2]
    const int * const src_fld, // [m2]
    const extractor_t * const extractors, // [m2]
    const char * const df_name, 
    const char ** const out_flds, // [m2] 
    int *ptr_hdr_len
    );
extern int
add_data(
    char *pkt, 
    const char ** const data_flds, 
    int m, 
    int n, 
    const qtype_t * const qtypes
    );
