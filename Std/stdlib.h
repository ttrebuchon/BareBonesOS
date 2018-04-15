#ifndef INCLUDED_STDLIB_H
#define INCLUDED_STDLIB_H

#include <Common.h>
C_CODE

long int strtol(const char* str, char** endptr, int base);

C_END
#endif