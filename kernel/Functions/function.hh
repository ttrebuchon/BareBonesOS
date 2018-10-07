#ifndef INCLUDED_KERNEL_FUNCTIONS_FUNCTION_HH
#define INCLUDED_KERNEL_FUNCTIONS_FUNCTION_HH

#include "function.h"

namespace Kernel
{
	inline int invoke_registered_fn(const sysfn_t* fn, auto cwd, int argc, const char** argv)
	{
		return fn->fn(argc, argv, cwd, fn);
	}
	
	
	/* Without process */
	
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**));
	
	// With istream
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&));
	
	//With ostream
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::ostream&, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::ostream&));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::ostream&, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::ostream&));
	
	// With istream and ostream
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&));
	
	
	
	
	
	
	
	
	
	/* With Process */
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, struct process_info*));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, struct process_info*));
	
	// With istream
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&, struct process_info*));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&, struct process_info*));
	
	//With ostream
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::ostream&, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::ostream&, struct process_info*));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::ostream&, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::ostream&, struct process_info*));
	
	// With istream and ostream
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&, struct process_info*));
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&, struct process_info*, void*), void* data);
	int init_system_fn(sysfn_t*, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&, struct process_info*));
	
	
	
	
	template <class... T>
	sysfn_t create_system_fn(int(*fn)(T...))
	{
		sysfn_t f;
		init_system_fn(&f, fn);
		return f;
	}
}

#endif