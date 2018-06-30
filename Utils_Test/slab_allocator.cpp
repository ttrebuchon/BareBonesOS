#include "Tests.hh"
#include <kernel/Memory/Allocators/slab_allocator.hh>
#include <list>
#include <kernel/Memory/Allocators/heap_allocator.hh>
#include <kernel/Memory/Heaps/BuddyHeap.hh>
#include <kernel/Memory/Allocators/slab_backup_allocator.hh>

struct some_obj
{
	char raw[32];
};

TEST(slab_allocator)
{
	using namespace Kernel::Memory;
	
	
	{
		const size_t mem_sz = 8192;
		typedef some_obj l_type;
		typedef slab_allocator<l_type, std::allocator<void>> lalloc_t;
		lalloc_t lalloc(mem_sz);
		
		typedef slab_allocator<uint8_t, std::allocator<void>> alloc_t;
		alloc_t salloc(lalloc);
		
		
		const size_t count = (mem_sz / (16*sizeof(l_type) + 6))*16;
		std::cout << "Count: " << count << std::endl;
		
		std::list<uint8_t, alloc_t> vec(salloc);
		
		for (int i = 0; i < count; ++i)
		{
			vec.push_back(i);
		}
		
		for (auto it = vec.begin(); it != vec.end(); ++it)
		{
			it = vec.erase(it);
		}
		
		assert(vec.size() > 0);
		
		for (auto i = vec.size(); i < count; ++i)
		{
			vec.push_back(i);
		}
		
		
	}
	
	QA::split_sections("BuddyHeap-backed slab_allocator");
	
	
	{
		typedef heap_allocator<void> meta_alloc;
		typedef std::allocator<void> meta_meta_allocator;
		const size_t meta_mem_sz = 16*64*1024;
		uint8_t mem[meta_mem_sz];
		
		
		auto buddy_heap = new BuddyHeap<meta_meta_allocator>(mem, meta_mem_sz, true, false);
		auto meta_heap = Utils::shared_ptr<BuddyHeap<meta_meta_allocator>>(buddy_heap);
		buddy_heap = nullptr;
		meta_alloc lalloc_meta(meta_heap);
		QA::out << "meta_heap max nodes: " << meta_heap->max_node_count() << std::endl;
		
		const size_t mem_sz = 8192;
		typedef some_obj l_type;
		typedef slab_allocator<l_type, meta_alloc> lalloc_t;
		QA::EnablePrintAllocs();
		
		lalloc_t lalloc(mem_sz, lalloc_meta);
		
		//static_assert(sizeof(SlabHeap<uint8_t, meta_alloc>) == 32);
		typedef slab_allocator<uint8_t, meta_alloc> alloc_t;
		alloc_t salloc(lalloc);
		
		
		const size_t count = (mem_sz / (16*sizeof(l_type) + 6))*16;
		QA::write_break() << "Count: " << count << std::endl;
		
		QA::out << "Creating list." << QA::br;
		std::list<uint8_t, alloc_t> vec(salloc);
		QA::out << "List allocated." << QA::br;
		
		for (int i = 0; i < count; ++i)
		{
			vec.push_back(i);
		}
		
		QA::out << "List filled." << QA::BR;
		
		for (auto it = vec.begin(); it != vec.end(); ++it)
		{
			it = vec.erase(it);
		}
		
		QA::out << "List partially erased." << QA::BR;
		
		assert(vec.size() > 0);
		
		for (auto i = vec.size(); i < count; ++i)
		{
			vec.push_back(i);
		}
		
		QA::out << "List refilled." << QA::BR;
		
	}
	
	QA::DisablePrintAllocs();
	
	
	QA::split_sections("Testing slab_backup_allocator");
	
	{
		
		typedef heap_allocator<uint8_t> meta_alloc;
		typedef heap_allocator<uint8_t> meta_meta_allocator;
		const size_t meta_mem_sz = 16*64*1024;
		
		uint8_t mem[meta_mem_sz];
		const size_t meta_mem_sz2 = SlabHeap_N<33>::Size_For_Count(BuddyHeap<>::pre_calc_max_node_count(mem, meta_mem_sz));
		uint8_t mem2[meta_mem_sz2];
		QA::out << "meta_mem_sz2: " << meta_mem_sz2 << std::endl;
		QA::out << "Potential Nodes: " << SlabHeap_N<33>::Available_Count(meta_mem_sz2) << std::endl;
		
		Utils::shared_ptr<Heap> meta_slab_heap = Utils::shared_ptr<Heap>(new SlabHeap_N<33>(mem2, meta_mem_sz2, false, false));
		heap_allocator<uint8_t> meta_slab_alloc(meta_slab_heap);
		Utils::shared_ptr<BuddyHeap<meta_meta_allocator>> buddy_heap(new BuddyHeap<meta_meta_allocator>(meta_slab_alloc, mem, meta_mem_sz, true, false));
		meta_alloc obj_alloc_meta(buddy_heap);
		QA::out << "meta_heap max nodes: " << buddy_heap->max_node_count() << std::endl;
		
		const size_t mem_sz = 8192;
		typedef some_obj l_type;
		typedef slab_backup_allocator<l_type, meta_alloc> obj_alloc_t;
		
		
		QA::EnablePrintAllocs();
		
		
		
		
		obj_alloc_t obj_alloc(mem_sz, obj_alloc_meta);
		
		typedef slab_backup_allocator<uint8_t, meta_alloc> alloc_t;
		alloc_t salloc(obj_alloc);
		
		
		const size_t count = (mem_sz / (16*sizeof(l_type) + 6))*16;
		QA::write_break() << "Count: " << count << std::endl;
		
		const size_t extra = 1000;
		QA::out << "Extra: " << extra << std::endl;
		QA::out << "Total: " << (count + extra) << QA::write_break;
		
		QA::out << "Creating list." << QA::br;
		std::list<uint8_t, alloc_t> vec(salloc);
		QA::out << "List allocated." << QA::br;
		
		for (int i = 0; i < count; ++i)
		{
			vec.push_back(i);
		}
		
		QA::out << "List filled from allocator." << QA::BR;
		
		for (int i = count; i < extra; ++i)
		{
			vec.push_back(i);
		}
		
		QA::out << "List filled." << QA::BR;
		
		for (auto it = vec.begin(); it != vec.end(); ++it)
		{
			it = vec.erase(it);
		}
		
		QA::out << "List partially erased." << QA::BR;
		
		assert(vec.size() > 0);
		
		for (auto i = vec.size(); i < count + extra; ++i)
		{
			vec.push_back(i);
		}
		
		QA::out << "List refilled." << QA::BR;
		
		
		
	}
	
	QA::DisablePrintAllocs();
}