#include "stdio.h"


C_CODE

FILE* stderr = nullptr;
FILE* stdout = nullptr;

int fprintf(FILE* fd, const char* fmt, ...)
{
	// TODO
	assert(false);
}

int printf(const char* fmt, ...)
{
	va_list ap;
	return fprintf(stdout, fmt, ap);
}


size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE*)
{
	// TODO
	assert(false);
}

int snprintf(char* s, size_t n, const char* fmt, ...)
{
	// TODO
	assert(false);
}


C_END