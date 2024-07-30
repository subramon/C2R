extern int
get_Rwidth(
    Rtype_t ttype
    );
extern int
get_width(
    qtype_t qtype
    );
extern int
roundup_16(
    int n
    );
extern int 
mk_hdr_len(
    int m1,
    int m2,
    const char * const df_name,
    const char ** const out_flds
    );
extern char *
set_n(
    char *pkt, 
    int n, 
    int m1, 
    int m2
    );
extern char *
set_q(
    char *pkt, 
    const qtype_t *const qtypes, 
    int m
    );
extern char *
set_R(
    char *pkt, 
    const Rtype_t *const rtypes, 
    int m
    );
extern char *
set_s(
    char *pkt, 
    const int *const src_flds, 
    int m
    );
extern char *
set_e(
    char *pkt, 
    const extractor_t *const extractors, 
    int m
    );
extern char *
set_f(
    char *pkt, 
    const char * const df_name,
    const char ** const out_flds,
    int m
    );
extern int
chk_src_flds(
    int *src_flds, // [m2]
    int m1,
    int m2
    );
extern int
chk_extractors(
    qtype_t *qtypes,
    Rtype_t *Rtypes,
    int *src_flds,
    extractor_t *extractors,
    int m1,
    int m2
    );
