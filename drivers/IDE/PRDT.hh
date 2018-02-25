#ifndef INCLUDED_PRDT_HH
#define INCLUDED_PRDT_HH

#include <Common.h>

namespace Drivers { namespace IDE {
	
	class PRD
	{
		public:
		// Buffer must be continuous in
		// physical memory and cannot
		// cross a 64K boundary
		uint32_t phys_buf;
		
		// 0 -> 64K
		uint16_t count;
		
		// 0, except for the last entry
		// where it should be
		// 0x1000 (MSB set)
		uint16_t end : 1;
		uint16_t reserved : 15;
	} __attribute__((packed));
	
	static_assert(sizeof(PRD) == 8);
	
	
	
	
	// Must be:
	// 32-bit aligned
	// Continuous in physical mem
	// Must not cross 64K border
	template <int N>
	class alignas(uint32_t) PRDT
	{
		private:
		
		public:
		PRD entries[N];
		
		static PRDT* Create(addr_t* phys = nullptr);
	};
}
}

#endif