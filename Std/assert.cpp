#include "assert.h"
#include "stdio.h"
#include <dlfcn.h>


C_CODE

#ifndef TESTING

void exit(int) __noreturn;

void __assert2(const char* filename, int line, const char* func, const char* exp)
{
	//int g = stderr;
	
	//fprintf(stderr, "%s:%d: %s: assertion "%s" failed\n", filename, line, func, exp);
	dlopen("Test", RTLD_NOW);
	
	//asm volatile("hlt 0");
	//exit(1);
}

#endif

C_END