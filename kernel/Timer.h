#ifndef INCLUDED_TIMER_H
#define INCLUDED_TIMER_H

#include <Common.h>
#include <Types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t useconds_t;


void init_timer(uint32_t freq);
unsigned int sleep(unsigned int seconds);
int usleep(useconds_t microseconds);

time_t time(time_t*);


#ifdef __cplusplus
}
#endif
#endif