#ifndef INCLUDED_MANAGERS_BASIC_PHYSICAL_HH
#define INCLUDED_MANAGERS_BASIC_PHYSICAL_HH

#include "PMM.hh"

namespace Kernel { namespace Memory
{
	
	
	class Basic_Physical : public PMM
	{
		
		
		public:
		
		Basic_Physical(frames_type&) noexcept;
		
		// Modifies size to equal the
		// the actual amount reserved
		// Guarantees "size" at
		// return is >= "size" input.
		// Starts looking forward at "hint"
		virtual addr_t reserve(size_t& size, addr_t hint) noexcept;
		
		// Modifies size to equal the
		// the actual amount reserved
		// Guarantees "size" at
		// return is >= "size" input
		virtual addr_t reserve(size_t& size) noexcept override final;
		
		// Returns size of freed memory
		virtual size_t release(addr_t loc, size_t count) noexcept;
		
	};
	
}
}


#endif