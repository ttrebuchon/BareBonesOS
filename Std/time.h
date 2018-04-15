#ifndef INCLUDED_TIME_H
#define INCLUDED_TIME_H

#include <Common.h>
#include "detail/tm.h"

#define EPOCH_YEAR 1970

C_CODE

time_t mktime(struct tm*);

struct tm* localtime(const time_t*);
struct tm* localtime_r(const time_t*, struct tm*);
time_t time(time_t*);

C_END

#endif