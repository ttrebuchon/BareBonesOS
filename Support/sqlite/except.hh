#ifndef INCLUDED_SUPPORT_SQLITE_EXCEPT_HH
#define INCLUDED_SUPPORT_SQLITE_EXCEPT_HH

#include <Common.h>
#include <Utils/detail/functexcept.hh>

extern "C" {
	struct sqlite3;
}

namespace Support { namespace SQLite
{
	
	void __throw_sqlite_error(const char*) __attribute__((noreturn));
	
	void __throw_sqlite_error(sqlite3*) __attribute__((noreturn));
	
	void __throw_null_statement_error(const char*) __attribute__((noreturn));
	
	#ifdef __EXCEPTIONS
	
	class sqlite_error : public Utils::exception
	{
		protected:
		Utils::string* _msg;
		public:
		explicit sqlite_error(const Utils::string&);
		explicit sqlite_error(const char*);
		
		sqlite_error(const sqlite_error&) throw();
		
		virtual ~sqlite_error() throw();
		
		
		sqlite_error& operator=(const sqlite_error&) throw();
		
		virtual const char* what() const throw();
	};
	
	#endif
	
}
}

#endif