#ifndef INCLUDED_BITS_DURATION_HPP
#define INCLUDED_BITS_DURATION_HPP

#include "duration.hh"

namespace Utils { namespace chrono
{
	template <class Rep, class Period>
	template <class Rep2>
	constexpr duration<Rep, Period>::duration(const Rep2& r) : ticks(r)
	{}
	
}
}

#endif