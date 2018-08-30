#include <sqlite3/sqlite3.h>
#include <Utils/detail/functexcept.hh>
#include <Utils/string>
#include "except.hh"

namespace Support { namespace SQLite
{
	#ifdef __EXCEPTIONS
	namespace detail
	{
		const char* get_ref_string(const Utils::string& str)
		{
			assert(NOT_IMPLEMENTED);
		}
	}
	
	sqlite_error::sqlite_error(const Utils::string& s) : _msg(new Utils::string(s))
	{}
	
	sqlite_error::sqlite_error(const char* s) : _msg(new Utils::string(s))
	{}
	
	sqlite_error::sqlite_error(const sqlite_error& e) throw() : _msg(e._msg ? new Utils::string(*e._msg) : nullptr)
	{}
	
	sqlite_error::~sqlite_error() throw()
	{
		if (_msg)
		{
			delete _msg;
		}
	}
	
	const char* sqlite_error::what() const throw()
	{
		return _msg->c_str();
	}
	
	#endif
	
	
	void __throw_sqlite_error(const char* s)
	{
		__THROW_OR_ABORT(sqlite_error(s));
	}
	
	
	void __throw_sqlite_error(sqlite3* sqlite)
	{
		__throw_sqlite_error(sqlite3_errmsg(sqlite));
	}
	
	void __throw_null_statement_error(const char* text)
	{
		__THROW_OR_ABORT(sqlite_error(text));
	}
}
}