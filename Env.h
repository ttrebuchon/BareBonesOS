#ifndef INCLUDED_ENV_H
#define INCLUDED_ENV_H



#if defined(__amd64__) || defined (__amd64) || defined(_x86_64__) || defined(_x86_64)
	
	#define __P_ARCH__ 64
	#define __ENV_x86__ 1
	#define __ENV__ x86
	
#elif defined(__aarch64__)

	#define __P_ARCH__ 64
	#define __ENV_AARCH64__ 1
	#define __ENV__ aarch64

#elif defined(_x86__) || defined(_x86_) || defined(_i386_) || defined(__i386__)

	#define __P_ARCH__ 32
	#define __ENV_x86__ 1
	#define __ENV__ x86
	
#endif


#ifndef __P_ARCH__
	
	#error Could not detect processor architecture
	
#endif

#if __P_ARCH__ == 32
	
	#define __ENV_32__ 1
	
#elif __P_ARCH__ == 64
	
	#define __ENV_64__ 1
	
#else
	
	#error Unknown architecture
	
#endif


#endif