#include "functexcept.hh"
#include "c++config.hh"

#ifdef _GLIBCXX_USE_NLS

//#include <libintl.h>
#define _(msgid) gettext(msgid)

#else

#define _(msgid) (msgid)

#endif

namespace Utils
{
	
	
	
	void __throw_length_error(const char* s __attribute__((unused)))
	{
		_GLIBCXX_THROW_OR_ABORT(length_error(_(s)));
	}
	
	void __throw_out_of_range(const char* s __attribute__((unused)))
	{
		_GLIBCXX_THROW_OR_ABORT(out_of_range(_(s)));
	}
	
}