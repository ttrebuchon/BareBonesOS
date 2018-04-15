#ifndef INCLUDED_STDIO_H
#define INCLUDED_STDIO_H

#include <Common.h>
#include <kernel/Filesystem/API.hh>

C_CODE

struct FILE;

extern FILE* stderr;
extern FILE* stdout;

int fprintf(FILE* fd, const char* fmt, ...);
int printf(const char* fmt, ...);

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE*);

int snprintf(char* s, size_t n, const char* fmt, ...);

C_END

#endif