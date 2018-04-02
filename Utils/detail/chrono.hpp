#ifndef INCLUDED_CHRONO_HPP
#define INCLUDED_CHRONO_HPP

#include "chrono.hh"

namespace Utils { namespace chrono
{
	
	
	
	
	
	
	template <class Clock, class Duration>
	constexpr auto time_point<Clock, Duration>::time_since_epoch() const -> duration
	{
		return counter;
	}
	
	
	
}
}

#endif