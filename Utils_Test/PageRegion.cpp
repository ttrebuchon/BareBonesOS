#include "Tests.hh"
#include <kernel/Memory/PageRegion.hh>
#include <kernel/Memory/PageDir.hh>


struct alignas(4096) some_align_struct
{
	double x, y, z;
};

TEST(page_region)
{
	auto pr = Kernel::Memory::PageDirectory::Regions.at(CODE_MEM_REGION);
	
	
	
	{
		Kernel::Memory::PageRegion_allocator<int> alloc(*pr);
		
		std::cout << "Allocating..." << std::endl;
		
		auto ptr = alloc.allocate(4);
		
		std::cout << "Allocated. [" << (void*)ptr << "]" << std::endl;
		
		alloc.deallocate(ptr, 4);
		
		std::cout << "Deallocated." << std::endl;
		
		assert(alloc == alloc);
		assert(!(alloc != alloc));
		
		std::cout << "Allocating..." << std::endl;
		Kernel::Memory::PageRegion_allocator<some_align_struct> alloc2(alloc);
		assert(alloc == alloc2);
		auto ptr2 = alloc2.allocate(1);
		
		std::cout << "Allocated. [" << (void*)ptr2 << "]" << std::endl;
		
		alloc2.deallocate(ptr2, 1);
		
		std::cout << "Deallocated." << std::endl;
	}
}