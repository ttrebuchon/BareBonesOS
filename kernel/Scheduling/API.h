#ifndef INCLUDED_SCHEDULING_API_H
#define INCLUDED_SCHEDULING_API_H

#include <Common.h>

C_CODE

int sched_yield();
int donate_priority(pthread_t);

C_END

#endif