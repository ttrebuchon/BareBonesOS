#ifndef INCLUDED_STDIO_H
#define INCLUDED_STDIO_H

#include <Common.h>

C_CODE

struct FILE;

extern FILE* stderr;
extern FILE* stdout;

int fprintf(FILE* fd, const char* fmt, ...);


C_END

#endif