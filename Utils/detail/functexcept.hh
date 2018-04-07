#ifndef INCLUDED_FUNCTEXCEPT_HH
#define INCLUDED_FUNCTEXCEPT_HH

namespace Utils
{
	#ifndef __EXCEPTIONS
	
	#define __try if (true)
	#define __catch(X) if (false)
	#define __throw_exception_again
	
	#else
	
	#define __try try
	#define __catch(X) catch (X)
	#define __throw_exception_again throw
	
	#endif
	
	void __throw_length_error(const char*) __attribute__((noreturn));
	void __throw_out_of_range(const char*) __attribute__((noreturn));
}

#endif