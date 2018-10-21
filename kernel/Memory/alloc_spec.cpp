#include "alloc_spec.h"

namespace Kernel::Memory
{

extern "C" {
	
	size_t alloc_spec_req_size(alloc_spec_t spec, size_t count)
	{
		if (spec.align <= 1)
		{
			return count*spec.size;
		}
		
		size_t p = 0;
		for (size_t i = 0; i < count; ++i)
		{
			p += spec.size;
			if (p % spec.align != 0)
			{
				p = ((p / spec.align) + 1)*spec.align;
			}
		}
		return p;
	}
}
}