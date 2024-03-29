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

void* memset(void* ptr, int value, size_t num);

void* memcpy(void*, const void*, size_t);


size_t strcspn(const char*, const char*);


size_t strlen(const char*);

char* strdup(const char*);

int strcmp(const char *p1, const char *p2);
int strncmp(const char* str1, const char* str2, size_t num);

char* strcpy(char* dest, const char* src);

#ifndef __cplusplus
char* strstr(const char* hay, const char* needle);
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" char* strstr(const char* hay, const char* needle);
//char* strstr(char* hay, const char* needle);
#endif

#endif