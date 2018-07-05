#ifndef INCLUDED_STDIO_H
#define INCLUDED_STDIO_H

#include <Common.h>
#include <kernel/Filesystem/API.hh>

C_CODE

#ifndef TESTING
typedef struct __sFILE FILE;


extern FILE* stderr;
extern FILE* stdout;
#endif

int fprintf(FILE* fd, const char* fmt, ...) __attribute__((__format__(__printf__, 2, 3)));

int printf(const char* fmt, ...) __attribute__((__format__(__printf__, 1, 2)));

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE*);

int snprintf(char* s, size_t n, const char* fmt, ...) __attribute__((__format__(__printf__, 3, 4)));

C_END

#endif