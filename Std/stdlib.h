#ifndef INCLUDED_STDLIB_H
#define INCLUDED_STDLIB_H

#include <Common.h>
#include <Std.h>
C_CODE

long int strtol(const char* str, char** endptr, int base);

int atoi(const char*);

#ifdef __cplusplus
[[ noreturn ]]
#endif
void abort();

#ifdef __cplusplus
[[ noreturn ]]
#endif
void exit(int);

C_END
#endif