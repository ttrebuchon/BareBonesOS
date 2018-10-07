#ifndef INCLUDED_KERNEL_FUNCTIONS_FUNCTION_MANAGER_HH
#define INCLUDED_KERNEL_FUNCTIONS_FUNCTION_MANAGER_HH

#include <Common.h>
#include "function.h"
#include <Utils/map>
#include <Utils/string>

namespace Kernel
{
	enum class sys_function_kind
	{
		STANDARD = 0x1,
	};
	
	class sys_functions
	{
		public:
		typedef struct sys_function function_type;
		typedef sys_function_kind function_kind_type;
		typedef size_t size_type;
		
		protected:
		Utils::map<Utils::string, function_type> funcs;
		
		public:
		
		
		void add_defaults();
		
		bool add_fn(const char* name, const function_type&);
		bool add_fn(const Utils::string&, const function_type&);
		size_type count(const char* name) const
		{ return funcs.count(name); }
		size_type count(const Utils::string& name) const
		{ return funcs.count(name); }
		
		
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream&, Utils::ostream&) const;
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream&) const;
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::ostream&) const;
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd) const;
		
		Utils::string exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream&, int* result_code) const;
		Utils::string exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, int* result_code) const;
		
		
		
		
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream&, Utils::ostream&, struct process_info*) const;
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream&, struct process_info*) const;
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::ostream&, struct process_info*) const;
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, struct process_info*) const;
		
		Utils::string exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream&, struct process_info*, int* result_code) const;
		Utils::string exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, struct process_info*, int* result_code) const;
		
		
		
		
		
		
		
		
		
		
		// streambuf args instead:
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, Utils::streambuf* out) const;
		
		Utils::string exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, int* result_code) const;
		
		int exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, Utils::streambuf* out, struct process_info*) const;
		
		Utils::string exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, struct process_info*, int* result_code) const;
	};
	
	
}

#endif