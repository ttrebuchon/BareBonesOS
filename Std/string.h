#ifndef INCLUDED_STD_STRING_H
#define INCLUDED_STD_STRING_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif

// Compares 2 memory blocks
// if *ptr1 < *ptr2: return < 0
// if *ptr1 > *ptr2 : return > 0
// if *ptr1 == *ptr2: return 0
int memcmp(const void*, const void*, size_t);

// Copy from src to dest as if by using a buffer
// to allow for accurate copying at
// overlapping blocks
void* memmove(void* dest, const void* src, size_t num);


size_t memcpy(void*, const void*, size_t);


size_t strcspn(const char*, const char*);


#ifdef __cplusplus
}
#endif

#endif