#ifndef INCLUDED_CWCHAR_HH
#define INCLUDED_CWCHAR_HH

#include <Common.h>

namespace Utils {
	
	struct mbstate_t
	{
		uint8_t __seq[4];
		char __reserved[4];
	};
	
	
}

#endif