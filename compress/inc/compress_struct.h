#include <stdbool.h>
#include <stdint.h>
#ifndef __COMPRESS_STRUCT_H
#define __COMPRESS_STRUCT_H

#define XT_ARRAY_INT     32 /* P  data: [n*4]int,int,.. */
#define XT_ARRAY_DOUBLE  33 /* P  data: [n*8]double,double,.. */
#define XT_ARRAY_BOOL    36 /* P  data: int(n),byte,byte,... */

typedef enum _Rtype_t { 
  R_DOUBLE = XT_ARRAY_DOUBLE,
  R_INTEGER = XT_ARRAY_INT,
  R_LOGICAL = XT_ARRAY_BOOL
} Rtype_t;

typedef enum _extractor_enum_t {
  UNDEF_EXTRACTOR,
  DEFAULT_EXTRACTOR, // means that no additional information needed

  EXTRACT_BITS,
  FP_TO_F8 // fixed point to double
  
} extractor_enum_t;

typedef struct _bit_spec_t {
  int start;
  int stop;
} bit_spec_t;

// For example, blfoat16
// (https://en.wikipedia.org/wiki/Bfloat16_floating-point_format)
// is_sign = true, exponent_bits = 8, fraction_bits = 7
typedef struct _fixed_point_t {
  bool is_sign; 
  uint8_t exponent_bits;
  uint8_t fraction_bits;
} fixed_point_t;

typedef struct _extractor_t {
  union {
    bit_spec_t bit_spec;
    fixed_point_t fp_spec;
  } extract_spec;
  extractor_enum_t extractor_enum;
} extractor_t;
 
#endif // __COMPRESS_STRUCT_H
