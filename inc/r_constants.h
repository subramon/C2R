#ifndef __R_CONSTANTS_H
#define __R_CONSTANTS_H
#undef WIN32
// #define NO_CONFIG_H
#include "Rsrv.h"
// The response contains a response code and any associated data. 

#define HDR_LEN (sizeof(struct phdr))
/*
The header is structured as follows:

  [0]  (int) command
  [4]  (int) length of the message (bits 0-31)
  [8]  (int) offset of the data part, 
    where 0 means directly after the header (which is normally the case)
  [12] (int) length of the message (bits 32-63)
  */
#endif // __R_CONSTANTS_H
