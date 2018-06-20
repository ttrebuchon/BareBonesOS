#include "stdio.h"


C_CODE

FILE* stderr = nullptr;
FILE* stdout = nullptr;

int vfprintf(FILE* fd, const char* fmt, va_list arg)
{
	assert(NOT_IMPLEMENTED);
}

int fprintf(FILE* fd, const char* fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	int res = vfprintf(fd, fmt, ap);
	va_end(ap);
	return res;
}

int printf(const char* fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	int res = vfprintf(stdout, fmt, ap);
	va_end(ap);
	return res;
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