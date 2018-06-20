#include <Common.h>
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
	
	auto ios_base::setf(fmtflags opts) -> fmtflags
	{
		auto old = _flags;
		_flags |= opts;
		return old;
	}
	
	auto ios_base::setf(fmtflags opts, fmtflags mask) -> fmtflags
	{
		auto old = _flags;
		_flags = ((mask & opts)|(_flags&~mask));
		return old;
	}
	
	void ios_base::unsetf(fmtflags opts)
	{
		_flags &= ~opts;
	}
	
	
	
	
	
	
	
	
	
	
	locale ios_base::getloc() const
	{
		return _loc;
	}
	
}