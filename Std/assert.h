#ifndef INCLUDED_ASSERT_H
#define INCLUDED_ASSERT_H



#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void __assert2(const char* filename, int line, const char* function, const char* exp);



#undef assert

#ifdef NDEBUG

	#define assert(ignored) ((void)0)
	
#else
	
	#define assert(exp) do { if (!(exp)) { __assert2(__FILE__, __LINE__, __func__, "assertion failed: \""#exp"\""); __builtin_unreachable();  } } while (0)
	
	
	
#endif




#ifdef __cplusplus
}
#endif

#endif