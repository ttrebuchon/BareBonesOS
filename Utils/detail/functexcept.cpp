#include "functexcept.hh"
#include "c++config.hh"
#include <Utils/string>

#ifdef _GLIBCXX_USE_NLS

//#include <libintl.h>
#define _(msgid) gettext(msgid)

#else

#define _(msgid) (msgid)

#endif

namespace Utils
{
	#ifdef __EXCEPTIONS
	namespace detail
	{
		const char* get_ref_string(const string& str)
		{
			
		}
	}
	
	logic_error::logic_error(const string& s) : _msg(new string(s))
	{}
	
	logic_error::logic_error(const char* s) : _msg(new string(s))
	{}
	
	logic_error::logic_error(const logic_error& e) throw() : _msg(e._msg ? new string(*e._msg) : nullptr)
	{}
	
	logic_error::~logic_error() throw()
	{
		if (_msg)
		{
			delete _msg;
		}
	}
	
	const char* logic_error::what() const throw()
	{
		return _msg->c_str();
	}
	
	#endif
	
	
	void __throw_length_error(const char* s __attribute__((unused)))
	{
		_GLIBCXX_THROW_OR_ABORT(length_error(_(s)));
	}
	
	void __throw_out_of_range(const char* s __attribute__((unused)))
	{
		_GLIBCXX_THROW_OR_ABORT(out_of_range(_(s)));
	}
	
}