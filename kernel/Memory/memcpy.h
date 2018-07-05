#ifndef INCLUDED_MEMCPY_H
#define INCLUDED_MEMCPY_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif

void* kmemcpy(void* dest, const void* src, size_t) __attribute__((__nonnull__(1, 2)));
void* memcpy(void* dest, const void* src, size_t) __attribute__((__nonnull__(1, 2)));

#ifdef __cplusplus
}
#endif
#endif