#include "Untyped_SlabHeap.hh"

namespace Kernel::Memory
{
	namespace untyped_slab_detail
	{
		
		#define __MASK(I) (1 << (15 - index))
		#define SPEC_GREATER(S) ((S).size > (S).align ? (S).size : (S).align)
		
		uint8_t* slab_t::allocate(bool* full)
		{
			assert(signature == SLAB_SIGNATURE);
			assert(full);
			*full = false;
			assert(free_count > 0);
			assert(free_count <= 16);
			assert(free_mask);
			
			uint8_t index;
			for (index = 0; index < 16; ++index)
			{
				if (free_mask & __MASK(index))
				{
					break;
				}
			}
			assert(index < 16);
			assert(((uint16_t)__MASK(index)) > 0);
			
			if (--free_count == 0)
			{
				*full = true;
			}
			free_mask &= ~__MASK(index);
			
			
			// Abusing the function a bit
			// here, but it is
			// technically correct
			return (region + alloc_spec_req_size(spec, index));
		}
		
		
		bool slab_t::free(uint32_t index, bool* empty)
		{
			assert(signature == SLAB_SIGNATURE);
			assert(empty);
			assert(index < 16);
			*empty = false;
			
			if (~free_mask & __MASK(index))
			{
				free_mask |= __MASK(index);
				if (++free_count == 16)
				{
					*empty = true;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		
	}
}