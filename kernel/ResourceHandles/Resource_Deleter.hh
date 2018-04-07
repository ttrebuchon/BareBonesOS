#ifndef INCLUDED_RESOURCE_DELETER_HH
#define INCLUDED_RESOURCE_DELETER_HH

#include <Common.h>

namespace Kernel
{
	class ResourceHandle;
	
	struct Resource_Deleter
	{
		void operator()(ResourceHandle*) const;
	};
}

#endif