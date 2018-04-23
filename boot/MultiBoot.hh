#ifndef INCLUDED_MULTIBOOT_HH
#define INCLUDED_MULTIBOOT_HH

#include <Common.h>
#include "multiboot.h"

/*extern "C" {
	
	struct multiboot;
	struct multiboot_mmap_item;
	typedef struct multiboot_mmap_item multiboot_mmap_t;
}*/

namespace boot
{
	class MultiBoot
	{
		public:
		typedef multiboot_mmap_t mmap_type;
		
		
		private:
		mutable size_t _avail_mem;
		
		protected:
		const multiboot* ptr;
		const mmap_type* mmap;
		size_t mmap_len;
		
		const mmap_type** free_ranges;
		size_t free_ranges_len;
		
		public:
		MultiBoot(const multiboot*) noexcept;
		
		size_t available_memory() const noexcept;
		
		const mmap_type* const* free_segments(size_t& len) const noexcept
		{
			len = free_ranges_len;
			return free_ranges;
		}
		
		const mmap_type* all_segments(size_t& len) const noexcept
		{
			len = mmap_len;
			return mmap;
		}
		
		const mmap_type* mmap_next(const mmap_type*) const noexcept;
	};
	
	
	extern MultiBoot* mboot;
}

#endif