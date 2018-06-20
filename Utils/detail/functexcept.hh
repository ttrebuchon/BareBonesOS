#ifndef INCLUDED_FUNCTEXCEPT_HH
#define INCLUDED_FUNCTEXCEPT_HH

#include <kernel/c_cpp.h>

#ifdef __EXCEPTIONS
	#include <Utils/stringfwd.hh>
	#include <Utils/exception>
#endif

namespace Utils
{
	#ifdef __EXCEPTIONS

	
	class logic_error : public Utils::exception
	{
		protected:
		string* _msg;
		public:
		explicit logic_error(const string&);
		explicit logic_error(const char*);
		
		logic_error(const logic_error&);
		
		virtual ~logic_error();
		
		
		logic_error& operator=(const logic_error&);
		
		virtual const char* what() const;
		
	};
	
	
	class length_error : public logic_error
	{
		
		
		public:
		explicit length_error(const string& s) : logic_error(s) {}
		explicit length_error(const char* s) : logic_error(s) {}
		
		virtual ~length_error() = default;
	};
	
	class out_of_range : public logic_error
	{
		
		
		public:
		explicit out_of_range(const string& s) : logic_error(s) {}
		explicit out_of_range(const char* s) : logic_error(s) {}
		
		virtual ~out_of_range() = default;
	};
	
	
	
	
	#endif
	
	void __throw_length_error(const char*) __attribute__((noreturn));
	void __throw_out_of_range(const char*) __attribute__((noreturn));
}

#endif