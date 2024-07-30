#ifndef __QTYPES_H
#define __QTYPES_H
// TODO: Keep in sync with lua/subtypes.lua
// START extract_for_qtypes.tex
typedef enum { 
  Q0, // mixed  must be first one 
  BL, // boolean (currently stored as I1)

  I1,
  I2,
  I4,
  I8,

  F4,
  F8,

  UI1,
  UI2,
  UI4,
  UI8,

  SC,  // constant length strings
  SV,  // variable length strings
  TM,  // time struct 
  HL,  // holiday bit mask 
  NUM_QTYPES // must be last one 
} qtype_t;
typedef uint32_t holiday_bmask_t;
// STOP extract_for_qtypes.tex
typedef struct _tm_t {
  int8_t tm_year;	/* Year	- 1900. TODO P4 Watch out for 2027!  */
  int8_t tm_mon;	/* Month.	[0-11] */
  int8_t tm_mday;	/* Day.		[1-31] */
  int8_t tm_hour;	/* Hours.	[0-23] */
  int8_t tm_min;	/* Minutes.	[0-59] */
  int8_t tm_sec;	/* Seconds.	[0-60] (1 leap second) */
  // UNUSED int8_t tm_wday;	/* Day of week.	[0-6] */
  int16_t tm_yday;	/* Days in year.[0-365]	*/

  /* Not being used 
  int tm_isdst;			// DST.		[-1/0/1]
# ifdef	__USE_MISC
  long int tm_gmtoff;		// Seconds east of UTC.  
  const char *tm_zone;		// Timezone abbreviation.  
# else
  long int __tm_gmtoff;		// Seconds east of UTC.  
  const char *__tm_zone;	// Timezone abbreviation.  
# endif
  */
} tm_t;
extern int
get_width_c_qtype(
      qtype_t qtype
    );
extern qtype_t
get_c_qtype(
    const char *const qtype
    );
#endif // __QTYPES_H
