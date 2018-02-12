#include "ios_base.hh"

namespace Utils {
	
	ios_base::ios_base() : _flags(0), _loc()
	{
		
	}
	
	typename ios_base::fmtflags ios_base::flags() const
	{
		return _flags;
	}
	
	auto ios_base::flags(fmtflags nflags) -> fmtflags
	{
		auto old = _flags;
		_flags = nflags;
		return old;
	}
	
	
	
	
	
	
	
	
	
	
	locale ios_base::getloc() const
	{
		return _loc;
	}
	
}