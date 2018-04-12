#ifndef INCLUDED_TM_H
#define INCLUDED_TM_H

#include <Common.h>

C_CODE

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  long int tm_gmtoff;
  const char* tm_zone;
};

C_END

#define TM_YEAR_BASE 1900

#endif