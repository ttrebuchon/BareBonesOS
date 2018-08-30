#ifndef INCLUDED_ASSERT_H
#define INCLUDED_ASSERT_H

#include <Common.h>

C_CODE

void __assert2(const char* filename, int line, const char* function, const char* exp) __attribute__((__noreturn__));


#ifdef assert
	#undef assert
#endif

#ifdef NDEBUG

	#define assert(ignored) ((void)0)
	
#else
	
	#define assert(exp) do { (exp) ? ((void)0) : __assert2(__FILE__, __LINE__, __func__, #exp); } while (0)
	
	
	
#endif




C_END

#endif