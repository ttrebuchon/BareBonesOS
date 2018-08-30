#include "Tests.hh"
#include <kernel/Memory/Heaps/KernelHeap.hh>
#include <kernel/Memory/Allocators/heap_allocator.hh>
#include <list>
#include <set>

TEST(kheap)
{
	const size_t msize = 4*1024*256;//1024;
	uint32_t buf[msize/4];
	using namespace Kernel::Memory;
	{
		
		QA::out << "Allocated buffer" << std::endl;
		std::set<void*> allocated;
		
		kernel_heap<std::allocator<void>> heap(buf, buf + msize/4, std::allocator<void>());
		
		auto ptr = (uint64_t*)heap.allocate(sizeof(uint64_t));
		
		*ptr = 0xDEADBABA;
		
		QA::out << ptr << std::endl;
		
		
		
		heap.free(ptr);
		
		const size_t count = 0.5*(msize/2) / (sizeof(void*)*4);
		QA::out << "Count: " << count << QA::br;
		
		uint64_t* ptrs[count];
		for (int i = 0; i < count; ++i)
		{
			ptrs[i] = (uint64_t*)heap.allocate(sizeof(uint64_t), alignof(uint64_t));
			assert(!allocated.count(ptrs[i]));
			allocated.insert(ptrs[i]);
			assert(ptrs[i]);
			*ptrs[i] = 0xDEADBABA;
		}
		
		QA::out << QA::br;
		
		for (int i = 0; i < count; ++i)
		{
			//QA::out << ptrs[i] << std::endl;
			assert(ptrs[i]);
			assert(*ptrs[i] == 0xDEADBABA);
			heap.free(ptrs[i]);
			assert(allocated.count(ptrs[i]));
			allocated.erase(ptrs[i]);
			ptrs[i] = nullptr;
		}
		
		QA::out << QA::br << "Creating heap allocator...\n" << QA::div << std::endl;
		
		
		heap_allocator<uint64_t, raw_ptr> halloc(&heap);
		
		std::list<uint64_t, heap_allocator<uint64_t, raw_ptr>> hlist(halloc);
		//std::list<uint64_t, Utils::allocator<uint64_t>> hlist;
		
		for (int i = 0; i < count; ++i)
		{
			hlist.push_back(i);
		}
		
		hlist.clear();
		ptr = (uint64_t*)heap.allocate(sizeof(uint64_t));
		allocated.insert(ptr);
		
		*ptr = 0xDEADBABA;
		
		QA::out << "Ptr: " << ptr << std::endl;
		
		QA::out << "buf: " << buf << std::endl;
		assert(((msize/4) + 1) % 0x1000 != 0);
		QA::out << heap.pageSize() << std::endl;
		assert(heap.pageSize() == 0x1000);
		auto nptr = heap.allocate((msize/4) + 1);
		QA::out << nptr << std::endl;
		assert(!allocated.count(nptr));
		QA::out << "Allocated size: " << heap.allocated_size(nptr) << std::endl;
		QA::out << "order: " << (int)detail::required_order(msize) << std::endl;
		assert(heap.allocated_size(nptr) >= ((msize/4) + 1));
		assert(nptr);
		
		heap.free(nptr);
		
		nptr = heap.allocate(0x10000, 0x1000);
		assert(nptr);
		heap.free(nptr);
		
		auto nptr2 = heap.allocate(0x10000, 0x1000);
		assert(nptr2);
		assert(nptr == nptr2);
		nptr2 = nptr2 = heap.allocate(0x10000, 0x1000);
		assert(nptr2);
		assert(nptr != nptr2);
		
		heap.free(nptr);
		heap.free(nptr2);
		
		
		
		
		
		QA::out << "Destroying heap" << std::endl;
	}
}