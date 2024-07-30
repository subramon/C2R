#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "macros.h"
#include "qtypes.h"
#include "compress_struct.h"
#include "convert.h"

int 
convert(
    const char *const in_data,
    qtype_t qtype,
    Rtype_t rtype,
    char *out_data,
    extractor_t e,
    int n
    )
{
  int status = 0;
  int8_t  *inI1ptr = (int8_t *)in_data;
  int16_t *inI2ptr = (int16_t *)in_data;
  int32_t *inI4ptr = (int32_t *)in_data;
  int64_t *inI8ptr = (int64_t *)in_data;

  uint8_t  *inUI1ptr = (uint8_t *)in_data;
  uint16_t *inUI2ptr = (uint16_t *)in_data;
  uint32_t *inUI4ptr = (uint32_t *)in_data;
  uint64_t *inUI8ptr = (uint64_t *)in_data;

  float   *inF4ptr = (float *)in_data;
  double  *inF8ptr = (double *)in_data;

  int8_t  *outI1ptr = (int8_t *)out_data;
  int32_t *outI4ptr = (int32_t *)out_data;
  double  *outF8ptr = (double *)out_data;

  // TODO P2 Lots of missing cases to be filled in 
  switch ( e.extractor_enum ) {
    case DEFAULT_EXTRACTOR :
      switch ( rtype ) { 
        case R_INTEGER : 
          switch ( qtype ) { 
            case I1 :
              for ( int i = 0; i < n; i++ ) { outI4ptr[i] = inI1ptr[i]; }  
              break;
            case I2 :
              for ( int i = 0; i < n; i++ ) { 
                outI4ptr[i] = inI2ptr[i]; 
              }
              break;
            default : 
              go_BYE(-1);
              break;
          }
          break; 
        case R_DOUBLE : 
          switch ( qtype ) { 
            case I1 :
              for ( int i = 0; i < n; i++ ) { outF8ptr[i] = inI4ptr[i]; }  
              break;
            case I2 :
              for ( int i = 0; i < n; i++ ) { outF8ptr[i] = inI4ptr[i]; }  
              break;
            case F4 :
              for ( int i = 0; i < n; i++ ) { outF8ptr[i] = inF4ptr[i]; }  
              break;
            default : 
              go_BYE(-1);
              break;
          }
      }
      break;
    case EXTRACT_BITS :
      outI1ptr = (int8_t *)out_data;
      int start = e.extract_spec.bit_spec.start;
      int stop  = e.extract_spec.bit_spec.stop;
      uint64_t mask; 
      switch ( rtype ) { 
        case R_LOGICAL : 
          mask = 1; 
          switch ( qtype ) { 
            case I1 : case UI1 : 
              {
                for ( int i = 0; i < n; i++ ) { 
                  uint64_t val = inUI1ptr[i];
                  outI1ptr[i] = (val >> start) & 0x1; 
                }
              }
              break;
            case I2 : case UI2 : 
              {
                for ( int i = 0; i < n; i++ ) { 
                  uint64_t val = inUI2ptr[i];
                  outI1ptr[i] = (val >> start) & 0x1; 
                }
              }
              break;
            case I4 : case UI4 : 
              {
                for ( int i = 0; i < n; i++ ) { 
                  uint64_t val = inUI4ptr[i];
                  outI1ptr[i] = (val >> start) & 0x1; 
                }
              }
              break;
            default :
              go_BYE(-1);
              break;
          }
          break;
        case R_INTEGER : 
          break;
        default : 
          go_BYE(-1);
          break;
      }
      break; 
    case FP_TO_F8 : 
      printf("TODO\n"); go_BYE(-1);
      break;
    default : 
      go_BYE(-1);
      break;
  }
BYE:
  return status;
}
