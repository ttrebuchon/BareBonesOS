#ifndef INCLUDED_INT_TO_STR_H
#define INCLUDED_INT_TO_STR_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif


int c_int_to_str(int n, char* cstr, unsigned int max_len);
int c_uint_to_str(uint64_t n, char* cstr, unsigned int max_len);

#ifdef __cplusplus
}
#endif

#endif