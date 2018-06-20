#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include <Utils/iostream>


C_CODE

#ifndef TESTING

void __do_kernel_panic(const char* msg, const char* file, const int line, const char* function);


void __assert2(const char* filename, int line, const char* func, const char* exp)
{
	__DO_CLI__
	if (!::stdlib_initialized())
	{
		__do_kernel_panic(exp, filename, line, func);
		__builtin_unreachable();
	}

	__try
	{
		std::cout << filename << "::" << line << "::" << func << "::" << exp << std::endl;
	}
	__catch(...)
	{
		__do_kernel_panic(exp, filename, line, func);
	}

	__do_kernel_panic_quiet();
	exit(-1);
}

#endif

C_END