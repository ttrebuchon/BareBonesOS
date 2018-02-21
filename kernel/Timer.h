#ifndef INCLUDED_TIMER_H
#define INCLUDED_TIMER_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif


void init_timer(uint32_t freq);
void sleep(uint32_t ms);


#ifdef __cplusplus
}
#endif
#endif