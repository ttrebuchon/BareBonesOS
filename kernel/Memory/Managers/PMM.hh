#ifndef INCLUDED_PMM_HH
#define INCLUDED_PMM_HH

#include <Common.h>
#include "../PhysicalMemory.hh"

namespace Kernel { namespace Memory
{
	
	class PMM
	{
		public:
		typedef typename PhysicalMemory::frames_type frames_type;
		
		protected:
		PMM(frames_type&) noexcept;
		
		frames_type& frames;
		
		public:
		
		// Modifies size to equal the
		// the actual amount reserved
		// Guarantees "size" at
		// return is >= "size" input
		virtual addr_t reserve(size_t& size) noexcept = 0;
		
		// Returns size of freed memory
		virtual size_t release(addr_t loc, size_t count) noexcept = 0;
		
		
	};
	
}
}

#endif