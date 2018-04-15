#include "assert.h"
#include "stdio.h"
#include "stdlib.h"


C_CODE

#ifndef TESTING

void __do_kernel_panic(const char* msg, const char* file, const int line, const char* function);


void __assert2(const char* filename, int line, const char* func, const char* exp)
{

	__do_kernel_panic(exp, filename, line, func);
	//int g = stderr;
	
	//fprintf(stderr, "%s:%d: %s: assertion "%s" failed\n", filename, line, func, exp);
	
	//asm volatile("hlt 0");
	exit(-1);
}

#endif

C_END