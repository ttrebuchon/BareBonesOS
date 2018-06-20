#include <kernel/Memory/PageDir.hh>
#include <kernel/Memory/Paging.hh>
#include <kernel/Memory/Heap.hh>


namespace Kernel { namespace Memory
{
	//PageDirectory* PageDirectory::Current = nullptr;
	
	//PageDirectory* kernel_dir = nullptr;
	
	Heap* kheap = nullptr;
	
	
	extern "C" void copy_page_physical(uint32_t, uint32_t)
	{}
	
}
}