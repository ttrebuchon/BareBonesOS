#include "chrono.hh"
#include "chrono.hpp"
#include <Std.h>

namespace Utils { namespace chrono
{
	
	
	
	
	
	
	
	
	
	
	
	auto system_clock::now() -> time_point
	{
		return time_point(chrono::seconds(time(nullptr)));
	}
	
	
}
}