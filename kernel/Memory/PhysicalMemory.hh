#ifndef INCLUDED_PHYSICAL_MEMORY_HH
#define INCLUDED_PHYSICAL_MEMORY_HH

#include <Common.h>

namespace Kernel { namespace Memory
{
	class PhysicalMemory final
	{
		private:
		constexpr static size_t pageSize = 0x1000;
		
		public:
		
		//  Modifies size to equal the
		// the actual amount reserved
		addr_t reserveFirstFree(size_t& size) noexcept;
		
		
		
	};
}
}

#endif