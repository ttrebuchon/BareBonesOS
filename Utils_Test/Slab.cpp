#include "Tests.hh"
#include <kernel/Memory/Heaps/SlabHeap.hh>
#include <kernel/Scheduling/thread.h>


TEST(slab)
{
	auto mem = new uint8_t[8192];
	
	{
		Kernel::Memory::SlabHeap<uint64_t> sheap(mem, 8192, false, false);
		std::cout << sheap.slab_count() << std::endl;
		assert(sheap.slab_count() == 60);
		
		auto p1 = sheap.alloc(sizeof(uint64_t));
		std::cout << p1 << std::endl;
		
		sheap.free(p1);
		p1 = sheap.alloc(sizeof(uint64_t));
		std::cout << p1 << std::endl;
		sheap.free(p1);
		
		uint64_t* ptrs[961];
		
		for (int i = 0; i < 960; ++i)
		{
			ptrs[i] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
			assert(ptrs[i]);
			*ptrs[i] = 0;
		}
		
		ptrs[960] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
		assert(!ptrs[960]);
		
		assert(sheap.startAddr() == (addr_t)mem);
		assert(sheap.endAddr() == (addr_t)(mem+8192));
		
		for (int i = 959; i >= 0; --i)
		{
			sheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
		
		for (int i = 0; i < 961; ++i)
		{
			assert(!ptrs[i]);
		}
		
		for (int i = 0; i < 960; ++i)
		{
			ptrs[i] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
			assert(ptrs[i]);
			*ptrs[i] = 0;
			if ((i % 16 == 15) || (i%16 == 0))
			{
				std::cout << i << " (" << (i % 16) << "): " << ptrs[i] << std::endl;
			}
		}
		
		ptrs[960] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
		assert(!ptrs[960]);
		
		for (int i = 0; i < 960; i += 3)
		{
			sheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
		
		for (int i = 319; i >= 0; --i)
		{
			assert(!ptrs[i*3]);
			ptrs[i*3] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
			assert(ptrs[i*3]);
			*ptrs[i*3] = 0;
			if ((i % 16 == 15) || (i%16 == 0))
			{
				std::cout << i << " (" << (i % 16) << "): " << ptrs[i*3] << std::endl;
			}
		}
		
		for (int i = 0; i < 960; ++i)
		{
			if (!ptrs[i])
			{
				std::cout << i << std::endl;
			}
			assert(ptrs[i]);
		}
		
		
		for (int i = 959; i >= 0; --i)
		{
			sheap.free(ptrs[i]);
			//sheap.free(ptrs[i]);
			ptrs[i] = nullptr;
			sheap.free(ptrs[i]);
		}
		
	}
	
	
	
	{
		
		Kernel::Memory::SlabHeap<uint64_t> sheap(mem, 8192, false, false);
		
		uint64_t* ptrs[960];
		memset(ptrs, 0, sizeof(ptrs));
		
		pthread_t ids[160];
		
		for (int i = 0; i < 160; ++i)
		{
			Kernel::thread_create(&ids[i], [=, &sheap, &ptrs]() -> void
			{
				for (int j = 0; j < 6; ++j)
				{
					ptrs[i*6+j] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
				}
				
				for (int j = 0; j < 6; ++j)
				{
					sheap.free(ptrs[i*6+j]);
				}
				
				for (int j = 0; j < 6; ++j)
				{
					ptrs[i*6+j] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
				}
			});
		}
		
		for (int i = 0; i < 160; ++i)
		{
			Kernel::thread_join(ids[i], nullptr);
		}
		
		for (int i = 0; i < 960; ++i)
		{
			if (!ptrs[i])
			{
				std::cout << i << std::endl;
			}
			assert(ptrs[i]);
		}
		
		for (int i = 0; i < 960; ++i)
		{
			sheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
	}
	
	delete[] mem;
	
	
	
	
	
	size_t mem_sz = 131072;
	mem = new uint8_t[mem_sz];
	
	
	
	{
		
		Kernel::Memory::SlabHeap<uint64_t> sheap(mem, mem_sz, false, false);
		
		const int ptr_count = sheap.slab_count()*16;
		std::cout << sheap.slab_count() << " slabs." << std::endl;
		
		uint64_t* ptrs[ptr_count];
		memset(ptrs, 0, sizeof(ptrs));
		
		const int ppt = 48;
		assert(ppt > 0);
		assert(ptr_count % ppt == 0);
		
		const int tcount = ptr_count / ppt;
		
		pthread_t ids[tcount];
		
		for (int i = 0; i < tcount; ++i)
		{
			Kernel::thread_create(&ids[i], [=, &sheap, &ptrs]() -> void
			{
				for (int j = 0; j < ppt; ++j)
				{
					ptrs[i*ppt+j] = (uint64_t*)sheap.alloc(sizeof(uint64_t));
				}
				
				for (int j = 0; j < ppt; ++j)
				{
					sheap.free(ptrs[i*ppt+j]);
				}
				
				for (int j = 0; j < ppt; ++j)
				{
					ptrs[i*ppt+j] = (uint64_t*)sheap.alloc(/*sizeof(uint64_t)*/1);
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
		
		assert(sheap.alloc(sizeof(uint64_t)) == nullptr);
		
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
			sheap.free(ptrs[i]);
			ptrs[i] = nullptr;
		}
	}
	
	
	delete[] mem;
	
}