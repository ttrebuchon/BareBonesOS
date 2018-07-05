#ifndef INCLUDED_MEMORY_PAGEREGIONS_DYNAMIC_CODE_REGION_HH
#define INCLUDED_MEMORY_PAGEREGIONS_DYNAMIC_CODE_REGION_HH

#include <Common.h>
#include "../PageRegion.hh"
#include <Utils/list>
#include "../PageDir.hh"
#include "../Allocators/cross_processor_allocator.hh"

namespace Kernel::Memory
{
	class Heap;
}



namespace Kernel::Memory::PageRegions
{
	
	class Dynamic_Code : public PageRegion
	{
		public:
		
		
		protected:
		typedef cross_proc_allocator<PageDirectory::Table*> table_allocator_type;
		typedef typename PageDirectory::Table table_type;
		
		Utils::list<table_type*, table_allocator_type> _tables;
		PageDirectory* kernel_dir;
		Heap* _heap;
		
		virtual uint8_t* allocate(size_t size, size_t alignment = 0) override;
		virtual void deallocate(void*, size_t size = 0) override;
		
		
		public:
		Dynamic_Code(PageDirectory* kernel_dir) noexcept __attribute__((__nonnull__(1)));
	};
	
	
};


#endif