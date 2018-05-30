#if __cplusplus >= 201703L
#ifndef INCLUDED_SUPPORT_SQLITE_CONTEXT_HPP
#define INCLUDED_SUPPORT_SQLITE_CONTEXT_HPP

#include "context.hh"

namespace Support { namespace SQLite
{
	
	template <class Derived, class Base>
	void SQLiteContext::add_inheritance()
	{
		update_inheritance_hierarchy(detail::BuiltModel<Derived>::name, detail::BuiltModel<Base>::name, Utils::is_abstract<Derived>::value);
		
		// TODO: Do other stuff
	}
	
}
}

#endif
#endif