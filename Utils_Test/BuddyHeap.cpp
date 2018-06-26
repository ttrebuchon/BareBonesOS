#include "Tests.hh"
#include <kernel/Memory/Heaps/BuddyHeap.hh>
#include <kernel/Scheduling/Thread.h>

TEST(buddy)
{
	auto mem = new uint8_t[8192];
	
	{
		Kernel::Memory::BuddyHeap<> bheap(mem, 8192, false, false, 4096, -1);
		
		
		auto p1 = bheap.alloc(sizeof(uint64_t));
		std::cout << p1 << std::endl;
		assert(!bheap.full());
		
		bheap.free(p1);
		p1 = bheap.alloc(sizeof(uint64_t));
		std::cout << p1 << std::endl;
		bheap.free(p1);
		assert(!bheap.full());
		
		size_t block_size = bheap.smallest_alloc();
		if (block_size < sizeof(uint64_t))
		{
			block_size = sizeof(uint64_t);
		}
		
		const int count = 8192 / block_size;
		std::cout << "Smallest Block: " << bheap.smallest_alloc() << std::endl;
		std::cout << "Count: " << count << std::endl;
		assert(count == 128);
		std::cout << "Start:\t" << (void*)bheap.startAddr() << std::endl;
		std::cout << "End:\t" << (void*)bheap.endAddr() << std::endl;
		std::cout << "Len:\t" <<  (void*)(bheap.endAddr() - bheap.startAddr()) << std::endl;
		
		std::cout << "Count:\t" << count << std::endl;
		
		uint64_t* ptrs[count+1];
		
		for (int i = 0; i < count; ++i)
		{
			assert(!bheap.full());
			ptrs[i] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
			assert(ptrs[i]);
			*ptrs[i] = 0;
		}
		
		std::cout << "All allocated." << std::endl;
		
		assert(bheap.full());
		std::cout << "Allocating one more..." << std::endl;
		ptrs[count] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
		assert(!ptrs[count]);
		
		for (int i = 0; i < count; ++i)
		{
			if (ptrs[i])
			{
				assert(((addr_t)ptrs[i] - bheap.startAddr()) % bheap.smallest_alloc() == 0);
			}
		}
		
		assert(bheap.startAddr() == (addr_t)mem);
		assert(bheap.endAddr() == (addr_t)(mem+8192));
		
		std::cout << "Freeing..." << std::endl;
		
		for (int i = count-1; i >= 0; --i)
		{
			assert(ptrs[i]);
			bheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
		
		assert(!bheap.full());
		
		for (int i = 0; i < count+1; ++i)
		{
			assert(!ptrs[i]);
		}
		
		std::cout << "Freed." << std::endl;
		
		for (int i = 0; i < count; ++i)
		{
			ptrs[i] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
			assert(ptrs[i]);
			*ptrs[i] = 0;
		}
		
		ptrs[count] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
		assert(!ptrs[count]);
		
		std::cout << "Allocated." << std::endl;
		
		for (int i = 0; i < count; ++i)
		{
			if (ptrs[i])
			{
				assert(((addr_t)ptrs[i] - bheap.startAddr()) % bheap.smallest_alloc() == 0);
			}
		}
		
		for (int i = 0; i < count; i += 3)
		{
			*ptrs[i] = 0xDEADBABA;
			bheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
		
		std::cout << "Partially Freed." << std::endl;
		
		for (int i = count-1; i >= 0; --i)
		{
			if (!ptrs[i])
			{
				ptrs[i] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
				assert(ptrs[i]);
				*ptrs[i] = 0;
			}
		}
		
		std::cout << "Reallocated." << std::endl;
		
		
		for (int i = 0; i < count; ++i)
		{
			if (!ptrs[i])
			{
				std::cout << i << std::endl;
			}
			assert(ptrs[i]);
		}
		
		for (int i = 0; i < count; ++i)
		{
			if (ptrs[i])
			{
				assert(((addr_t)ptrs[i] - bheap.startAddr()) % bheap.smallest_alloc() == 0);
				assert(bheap.allocated_size(ptrs[i]) == bheap.smallest_alloc());
			}
		}
		
		
		for (int i = 0; i < count; ++i)
		{
			assert(((addr_t)ptrs[i] - bheap.startAddr()) % bheap.smallest_alloc() == 0);
			*ptrs[i] = 0xDEADBABA;
			
			bheap.free(ptrs[i]);
			//bheap.free(ptrs[i]);
			ptrs[i] = nullptr;
			bheap.free(ptrs[i]);
		}
		
		std::cout << "Freed." << std::endl;
		
	}
	
	
	
	{
		
		Kernel::Memory::BuddyHeap<> bheap(mem, 8192, false, false, 4096, 0.0);
		
		size_t block_size = bheap.smallest_alloc();
		if (block_size < sizeof(uint64_t))
		{
			block_size = sizeof(uint64_t);
		}
		
		const int count = 8192 / block_size;
		
		uint64_t* ptrs[count];
		memset(ptrs, 0, sizeof(ptrs));
		
		const int tcount = 64;
		const int ppt = count / tcount;
		assert(ppt * tcount == count);
		assert(ppt > 0);
		assert(count % ppt == 0);
		
		pthread_t ids[tcount];
		
		for (int i = 0; i < tcount; ++i)
		{
			Kernel::thread_create(&ids[i], [=, &bheap, &ptrs]() -> void
			{
				for (int j = 0; j < ppt; ++j)
				{
					ptrs[i*ppt+j] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
				}
				
				for (int j = 0; j < ppt; ++j)
				{
					bheap.free(ptrs[i*ppt+j]);
				}
				
				for (int j = 0; j < ppt; ++j)
				{
					ptrs[i*ppt+j] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
				}
			});
		}
		
		for (int i = 0; i < tcount; ++i)
		{
			Kernel::thread_join(ids[i], nullptr);
		}
		
		for (int i = 0; i < count; ++i)
		{
			if (!ptrs[i])
			{
				std::cout << i << std::endl;
			}
			assert(ptrs[i]);
		}
		
		for (int i = 0; i < count; ++i)
		{
			bheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
	}
	
	delete[] mem;
	
	
	
	
	
	size_t mem_sz = 131072;
	mem = new uint8_t[mem_sz];
	
	
	
	{
		
		Kernel::Memory::BuddyHeap<> bheap(mem, mem_sz, false, false, 4096, 0.5);
		
		size_t block_size = bheap.smallest_alloc();
		if (block_size < sizeof(uint64_t))
		{
			block_size = sizeof(uint64_t);
		}
		
		const int ptr_count = mem_sz / block_size;
		
		uint64_t* ptrs[ptr_count];
		memset(ptrs, 0, sizeof(ptrs));
		
		const int ppt = 128;
		assert(ppt > 0);
		assert(ptr_count % ppt == 0);
		
		const int tcount = ptr_count / ppt;
		
		pthread_t ids[tcount];
		
		for (int i = 0; i < tcount; ++i)
		{
			Kernel::thread_create(&ids[i], [=, &bheap, &ptrs]() -> void
			{
				for (int j = 0; j < ppt; ++j)
				{
					ptrs[i*ppt+j] = (uint64_t*)bheap.alloc(sizeof(uint64_t));
				}
				
				for (int j = 0; j < ppt; ++j)
				{
					bheap.free(ptrs[i*ppt+j]);
				}
				
				for (int j = 0; j < ppt; ++j)
				{
					ptrs[i*ppt+j] = (uint64_t*)bheap.alloc(1);
				}
			});
		}
		
		__sync_synchronize();
		
		for (int i = tcount-1; i >= 0; --i)
		{
			Kernel::thread_join(ids[i], nullptr);
		}
		
		__sync_synchronize();
		
		std::clog << "Threads done!\n";
		
		assert(bheap.alloc(sizeof(uint64_t)) == nullptr);
		
		for (int i = 0; i < ptr_count; ++i)
		{
			if (!ptrs[i])
			{
				std::cout << i << std::endl;
			}
			assert(ptrs[i]);
		}
		
		for (int i = 0; i < ptr_count; ++i)
		{
			bheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
	}
	
	
	delete[] mem;
}