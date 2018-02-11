#include "ios_base.hh"

namespace Utils {
	
	ios_base::ios_base() : _flags(0), _loc()
	{
		
	}
	
	typename ios_base::fmtflags ios_base::flags() const
	{
		return _flags;
	}
	
	
	
	
	
	
	
	
	
	
	locale ios_base::getloc() const
	{
		return _loc;
	}
	
}