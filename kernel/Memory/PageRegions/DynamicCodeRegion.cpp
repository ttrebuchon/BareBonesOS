#include "DynamicCodeRegion.hh"
#include <kernel/Memory/Heaps/PageHeap.hh>


namespace Kernel::Memory::PageRegions
{
	
	Dynamic_Code::Dynamic_Code(PageDirectory* kernel_dir) noexcept : _tables(), kernel_dir(kernel_dir), _heap(nullptr)
	{
		cross_proc_allocator<PageHeap<cross_proc_allocator<void>>> halloc;
		auto heap_ptr = halloc.allocate(1);
		halloc.construct(heap_ptr, kernel_dir, true, false);
		_heap = heap_ptr;
	}
	
	
	uint8_t* Dynamic_Code::allocate(size_t size, size_t alignment)
	{
		return (uint8_t*)_heap->alloc(size, alignment);
	}
	
	void Dynamic_Code::deallocate(void* ptr, size_t size)
	{
		_heap->free(ptr);
	}
}