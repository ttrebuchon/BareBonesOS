#include "Paging.h"
#include <kernel/utils/Bitset.hh>
#include "kheap.h"

Utils::Bitset_Ptr<>* frame_collection;

extern "C" {

	void init_frame_collection(uint32_t size)
	{
		typedef typename Utils::Bitset_Ptr<>::storage Unit_t;
		
		frame_collection = (Utils::Bitset_Ptr<>*)kmalloc(sizeof(Utils::Bitset_Ptr<>), 1, 0);
		auto bits = (Unit_t*)kmalloc(size, 1, 0);
		frame_collection = new (frame_collection) Utils::Bitset_Ptr<>(bits, size/sizeof(Unit_t));
		
		
	}
	
	
	int first_frame(uint32_t* index)
	{
		return first_frame_from(index, 0);
	}
	
	
	int first_frame_from(uint32_t* index, uint32_t start)
	{
		if (frame_collection->firstFalse(index, start))
		{
			return 1;
		}
		return 0;
	}
	
	void set_frame(uint32_t addr)
	{
		frame_collection->set(addr, true);
	}
	
	void clear_frame(uint32_t addr)
	{
		frame_collection->set(addr, false);
	}
	
	
	uint32_t check_frame(uint32_t addr)
	{
		return frame_collection->get(addr);
	}
}