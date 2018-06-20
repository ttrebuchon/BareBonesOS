#ifndef INCLUDED_C_CPP_H
#define INCLUDED_C_CPP_H


#ifdef __cplusplus

	#define CPP_STATIC static
	#define EXTERN_C extern "C"
	#define START_NS(X) namespace X {
	#define END_NS }



	#ifndef __catch
		#ifdef __EXCEPTIONS
			#define __catch(...) catch(__VA_ARGS__)
		#else
			#define __catch(...) if (false)
		#endif
	#endif

	#ifndef __try
		#ifdef __EXCEPTIONS
			#define __try try
		#else
			#define __try if (true)
		#endif
	#endif

	#ifndef __throw_exception_again
		#ifdef __EXCEPTIONS
			#define __throw_exception_again throw
		#else
			#define __throw_exception_again
		#endif
	#endif



	#if __cplusplus >= 201703L
		#define IFCPP17(X) X
	#else
		#define IFCPP17(X)
	#endif




#else


	#define CPP_STATIC
	#define EXTERN_C 
	#define START_NS(X) 
	#define END_NS

#endif





#endif