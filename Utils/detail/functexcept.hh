#ifndef INCLUDED_FUNCTEXCEPT_HH
#define INCLUDED_FUNCTEXCEPT_HH

#ifdef __EXCEPTIONS
	#include <Utils/stringfwd.hh>
	#include <Utils/exception>
#endif

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
	
	class logic_error : public std::exception
	{
		protected:
		string* _msg;
		public:
		explicit logic_error(const string&);
		explicit logic_error(const char*);
		
		logic_error(const logic_error&) throw();
		
		virtual ~logic_error() throw();
		
		
		logic_error& operator=(const logic_error&) throw();
		
		virtual const char* what() const throw();
		
	};
	
	
	class length_error : public logic_error
	{
		
		
		public:
		explicit length_error(const string& s) : logic_error(s) {}
		explicit length_error(const char* s) : logic_error(s) {}
		
		virtual ~length_error() throw() = default;
	};
	
	class out_of_range : public logic_error
	{
		
		
		public:
		explicit out_of_range(const string& s) : logic_error(s) {}
		explicit out_of_range(const char* s) : logic_error(s) {}
		
		virtual ~out_of_range() throw() = default;
	};
	
	
	
	
	#endif
	
	void __throw_length_error(const char*) __attribute__((noreturn));
	void __throw_out_of_range(const char*) __attribute__((noreturn));
}

#endif