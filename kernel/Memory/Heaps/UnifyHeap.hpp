#ifndef INCLUDED_MEMORY_HEAPS_UNIFY_HEAP_HPP
#define INCLUDED_MEMORY_HEAPS_UNIFY_HEAP_HPP

#include "UnifyHeap.hh"
#include <kernel/Memory/PageDir.hh>

#define UNIFYHEAP_MAGIC 0xDEADBABA

namespace Kernel::Memory
{
	inline bool verify_slot(unify_heap_detail::HeapHeader* hd)
	{
		typedef unify_heap_detail::HeapHeader head_type;
		typedef unify_heap_detail::HeapFooter foot_type;
		if (!hd)
		{
			return false;
		}
		assert(hd);
		if (hd->magic != UNIFYHEAP_MAGIC)
		{
			return false;
		}
		
		if (!(hd->size > sizeof(head_type) + sizeof(foot_type)))
		{
			return false;
		}
		
		auto ft = (foot_type*)(((addr_t)hd) + hd->size - sizeof(foot_type));
		
		if (ft->magic != UNIFYHEAP_MAGIC)
		{
			return false;
		}
		
		if (ft->header != hd)
		{
			return false;
		}
		
		
		
		
		return true;
	}
	
	
	
	template <class MA>
	UnifyHeap<MA>::UnifyHeap(PageDirectory* dir, bool kernel_mem, bool default_read_only, size_t pg_sz) noexcept : Heap(0, 0, pg_sz != 0 ? pg_sz : PAGE_SIZE, kernel_mem, default_read_only), mem_alloc(), memory(mem_alloc.allocate(1024)), memory_size(1024), index(memory, memory_size), _dir(dir)
	{
		
	}
	
	
	template <class MA>
	UnifyHeap<MA>::~UnifyHeap()
	{
		// Start from the end for OrderedArray efficiency
		size_t i;
		while ((i = index.size()) > 0)
		{
			auto head = index[i - 1];
			auto ptr = (void*)(((addr_t)head) + sizeof(*head));
			this->free(ptr);
		}
		
		assert(memory);
		mem_alloc.deallocate(memory, memory_size);
	}
	
	
	template <class MA>
	void* UnifyHeap<MA>::alloc(size_t size, size_t alignment)
	{
		if (size == 0)
		{
			return nullptr;
		}
		
		if (alignment == 0)
		{
			alignment = sizeof(void*);
			while (alignment < size)
			{
				alignment <<= 1;
				assert(alignment != 0);
				if (!(alignment > 1))
				{
					TRACE(alignment);
					TRACE(size);
				}
				assert(alignment > 1);
			}
		}
		
		
		
		head_type* addr = nullptr;
		
		int i;
		
		for (i = 0; i < index.size(); ++i)
		{
			assert(verify_slot(index[i]));
			if (index[i]->size >= size)
			{
				auto item = index[i];
				assert(item->is_hole);
				addr_t adjusted = (addr_t)(item + 1);
				if (adjusted % alignment != 0)
				{
					adjusted += (alignment - (adjusted % alignment));
				}
				
				assert(adjusted % alignment == 0);
				if (item->size - (adjusted - (addr_t)(item+1)) < size)
				{
					continue;
				}
				
				addr = item;
				break;
			}
		}
		
		if (addr)
		{
			assert(verify_slot(index[i]));
			index.erase(i);
			addr->is_hole = false;
			assert(addr->magic == UNIFYHEAP_MAGIC);
			assert(addr->size - sizeof(foot_type) - sizeof(head_type) >= size);
			assert(verify_slot(addr));
			addr_t adjusted = (addr_t)(addr + 1);
			if (adjusted % alignment != 0)
			{
				adjusted += (alignment - (adjusted % alignment));
				
			}
			if (alignment)
			{
				assert(adjusted % alignment == 0);
			}
			assert(adjusted - (addr_t)addr < max_adjust);
			return (void*)adjusted;
		}
		
		size_t need_size = size + sizeof(head_type) + sizeof(foot_type);
		
		size_t page_count = need_size / pageSize();
		if (need_size % pageSize() != 0)
		{
			++page_count;
		}
		
		if (alignment % sizeof(head_type) != 0)
		{
			auto nstart = sizeof(head_type);
			nstart += (alignment - (nstart % alignment));
			assert(nstart % alignment == 0);
			if (nstart + need_size > page_count*pageSize())
			{
				auto diff = (nstart + need_size - page_count*pageSize());
				page_count += diff / pageSize();
				if (diff % pageSize())
				{
					++page_count;
				}
			}
		}
		
		
		auto pg = this->alloc_page(page_count);
		assert(pg);
		
		addr_t seg_start, seg_end;
		
		
		seg_start = (addr_t)pg;
		seg_end = (addr_t)pg + pageSize()*page_count;
		{
			auto astart = ((addr_t)pg) + sizeof(head_type);
			if (astart % alignment != 0)
			{
				astart += (alignment - (astart % alignment));
			}
			assert(astart % alignment == 0);
			seg_start = astart - sizeof(head_type);
		}
		assert(seg_start - (addr_t)pg + size + sizeof(head_type) + sizeof(foot_type) <= page_count * pageSize());
		
		if (seg_end - seg_start - size > 2*(sizeof(head_type) + sizeof(foot_type)))
		{
			auto next_seg = (head_type*)(seg_start + (sizeof(head_type) + sizeof(foot_type)) + size);
			next_seg->magic = UNIFYHEAP_MAGIC;
			next_seg->is_hole = 1;
			auto next_foot = (foot_type*)((addr_t)pg + page_count*pageSize() - sizeof(foot_type));
			assert((addr_t)next_foot > (addr_t)next_seg);
			next_foot->magic = UNIFYHEAP_MAGIC;
			next_foot->header = next_seg;
			next_seg->size = ((addr_t)(next_foot+1) - (addr_t)next_seg);
			assert(verify_slot(next_seg));
			index.insert(next_seg);
			
			
			
			seg_end = (addr_t)next_seg;
			
		}
		else
		{
			size = (((addr_t)seg_end - (addr_t)seg_start) - (sizeof(head_type) + sizeof(foot_type)));
		}
		
		/*if (((addr_t)pg + page_count * pageSize()) - seg_end <= sizeof(head_type) + sizeof(foot_type))
		{
			seg_end = (addr_t)pg + page_count * pageSize();
			need_size = seg_end - seg_start;
		}
		else
		{
			auto next_seg = ((head_type*)seg_end);
			next_seg->magic = UNIFYHEAP_MAGIC;
			next_seg->is_hole = 1;
			auto next_foot = ((foot_type*)(((addr_t)pg) + pageSize()*page_count))-1;
			next_foot->magic = UNIFYHEAP_MAGIC;
			next_foot->header = next_seg;
			
			next_seg->size = (addr_t)next_foot - (addr_t)next_seg;
			
			index.insert(next_seg);
		}*/
		
		if (seg_start - (addr_t)pg > sizeof(head_type) + sizeof(foot_type))
		{
			auto next_seg = ((head_type*)pg);
			next_seg->magic = UNIFYHEAP_MAGIC;
			next_seg->is_hole = 1;
			auto next_foot = ((foot_type*)seg_start)-1;
			next_foot->magic = UNIFYHEAP_MAGIC;
			next_foot->header = next_seg;
			
			next_seg->size = seg_start - (addr_t)pg;
			assert(next_seg->size > sizeof(head_type) + sizeof(foot_type));
			assert(verify_slot(next_seg));
			index.insert(next_seg);
		}
		
		
		addr = (head_type*)seg_start;
		assert(addr);
		addr->magic = UNIFYHEAP_MAGIC;
		addr->size = seg_end - seg_start;
		assert(addr->size - sizeof(head_type) - sizeof(foot_type) == size);
		addr->is_hole = 0;
		auto ft = ((foot_type*)seg_end) - 1;
		ft->magic = UNIFYHEAP_MAGIC;
		ft->header = addr;
		
		assert((addr_t)addr + addr->size == seg_end);
		
		assert(verify_slot(addr));
		
		#ifdef DEBUG
		{
			auto rfoot = (foot_type*)((addr_t)addr + addr->size - sizeof(foot_type));
			assert(rfoot == ft);
			assert(((foot_type*)((addr_t)addr + addr->size - sizeof(foot_type))) == rfoot);
			assert(rfoot->magic == UNIFYHEAP_MAGIC);
			assert(rfoot->header == addr);
		}
		#endif
		
		assert((addr_t)(addr+1) % alignment == 0);
		
		assert((addr_t)ft - (addr_t)(addr+1) >= size);
		
		if (alignment > 0)
		{
			assert((uintptr_t)(void*)(addr+1) % alignment == 0);
			assert(alignment > 1);
		}
		
		return (void*)(addr+1);
	}
	
	
	
	
	
	template <class MA>
	void UnifyHeap<MA>::free(void* ptr)
	{
		if (!ptr)
		{
			return;
		}
		assert(ptr);
		auto head = ((unify_heap_detail::HeapHeader*)ptr - 1);
		
		if (head->magic != UNIFYHEAP_MAGIC)
		{
			for (int i = 0; i < max_adjust && ((addr_t)ptr - i) >= startAddr(); ++i)
			{
				if (*(uint32_t*)((addr_t)ptr - i) == UNIFYHEAP_MAGIC)
				{
					auto nhead = (head_type*)((addr_t)ptr - i);
					assert(nhead->magic == UNIFYHEAP_MAGIC);
					if (!nhead->is_hole)
					{
						if ((addr_t)nhead + nhead->size >= (addr_t)ptr)
						{
							ptr = (void*)(nhead+1);
							head = nhead;
							break;
						}
					}
				}
			}
		}
		
		#ifdef TESTING
		if (head->magic != UNIFYHEAP_MAGIC)
		{
			
			TRACE("Head Sentinel: ");
			TRACE((void*)(addr_t)head->magic);
			TRACE("Ptr: ");
			TRACE(ptr);
			//free(ptr);
			//return;
		}
		#endif
		assert(head->magic == UNIFYHEAP_MAGIC);
		auto foot = (unify_heap_detail::HeapFooter*)((addr_t)head + head->size - sizeof(unify_heap_detail::HeapFooter));
		#ifdef TESTING
		if (foot->magic != UNIFYHEAP_MAGIC)
		{
			
			TRACE("Foot Sentinel: ");
			TRACE((void*)(addr_t)foot->magic);
			TRACE("Ptr: ");
			TRACE(ptr);
		}
		#endif
		assert(foot->magic == UNIFYHEAP_MAGIC);
		assert(foot->header == head);
		assert(!head->is_hole);
		unify_heap_detail::HeapFooter* prev_foot = nullptr;
		head->is_hole = 1;
		bool add = true;
		if ((addr_t)head % this->pageSize() != 0)
		{
			prev_foot = ((unify_heap_detail::HeapFooter*)head - 1);
			if (prev_foot->magic == UNIFYHEAP_MAGIC)
			{
			auto prev_head = prev_foot->header;
			assert(verify_slot(prev_head));
			assert(prev_head->magic == UNIFYHEAP_MAGIC);
			if (prev_head->is_hole)
			{
				for (size_t i = 0; i < index.size(); ++i)
				{
					assert(verify_slot(index[i]));
					if (index[i] == prev_head)
					{
						index.erase(i);
						break;
					}
				}
				prev_head->size += head->size/* + sizeof(unify_heap_detail::HeapHeader) + sizeof(unify_heap_detail::HeapFooter)*/;
				foot->header = prev_head;
				head = prev_head;
				assert(verify_slot(head));
				assert(head->is_hole);
			}
			
			}
			else
			{
				head = (head_type*)((addr_t)head - ((addr_t)head % pageSize()));
				head->size = ((addr_t)foot - (addr_t)head + sizeof(foot_type));
				head->magic = UNIFYHEAP_MAGIC;
				head->is_hole = 1;
				foot->header = head;
				assert(verify_slot(head));
			}
		}
		
		auto next_head = (unify_heap_detail::HeapHeader*)(foot+1);
		if (((addr_t)head / pageSize()) == ((addr_t)next_head / pageSize()))
		{
			if (next_head->magic == UNIFYHEAP_MAGIC && next_head->is_hole)
			{
				auto next_foot = (unify_heap_detail::HeapFooter*)((addr_t)next_head + next_head->size - sizeof(unify_heap_detail::HeapFooter));
				if (next_foot->magic == UNIFYHEAP_MAGIC && next_foot->header == next_head)
				{
					for (size_t i = 0; i < index.size(); ++i)
					{
						assert(verify_slot(index[i]));
						if (index[i] == next_head)
						{
							index.erase(i);
							break;
						}
					}
					
					head->size += (next_head->size/* + sizeof(unify_heap_detail::HeapHeader) + sizeof(unify_heap_detail::HeapFooter)*/);
					foot = next_foot;
					foot->header = head;
					assert(verify_slot(head));
					assert(head->is_hole);
					
				}
			}
		}
		
		if (add)
		{
			assert(verify_slot(head));
			assert(head->is_hole);
			bool res = index.insert(head);
			assert(res);
		}
	}
	
	template <class MA>
	size_t UnifyHeap<MA>::allocated_size(void* ptr) const noexcept
	{
		assert(ptr);
		auto head = ((unify_heap_detail::HeapHeader*)ptr - 1);
		assert(head->magic == UNIFYHEAP_MAGIC);
		assert(!head->is_hole);
		return head->size - sizeof(head_type) - sizeof(foot_type);
	}
	
	
	template <class MA>
	void* UnifyHeap<MA>::alloc_page(size_t continuous_count)
	{
		assert(_dir);
		void* last = nullptr;
		
		for (int i = 0; i < index.size(); ++i)
		{
			auto addr = (void*)(((addr_t)index[i] / pageSize()) * pageSize());
			if (addr == last)
			{
				continue;
			}
			
			//auto tIndex = PageDirectory::GetTableIndex(addr);
			auto table = _dir->table(addr);
			assert(table);
			assert((*table)->present);
			
			for (int j = 0; j < table->page_count(); ++j)
			{
				auto& pg = table->at(j);
				if (!pg.present())
				{
					bool found = true;
					for (int k = 1; k < continuous_count && found && (j+k < table->page_count()); ++k)
					{
						auto& k_pg = table->at(j + k);
						if (k_pg.present())
						{
							found = false;
						}
					}
					
					if (found)
					{
						for (int k = 0; k < continuous_count; ++k)
						{
							auto& kpg = table->at(j+k);
							if (!kpg.allocate(!readonly(), supervisor()))
							{
								assert(false);
							}
						}
						
						return pg.virtual_address();
					}
				}
			}
			
			
			if (!last)
			{
				last = addr;
			}
			else
			{
				break;
			}
		}
		
		
		int empty_index = -1;
		
		assert(_dir->table_count() > 0);
		
		// First table is reserved
		for (int i = 1; i < _dir->table_count(); ++i)
		{
			auto tbl = _dir->table(i);
			if (empty_index == -1 && !tbl)
			{
				empty_index = i;
			}
			else if (tbl)
			{
				assert(tbl->page_count() > 0);
				for (int j = 0; j < tbl->page_count(); ++j)
				{
					auto& pg = tbl->at(j);
					if (!pg.present())
					{
						bool found = true;
						for (int k = 1; k < continuous_count && found && (j+k < tbl->page_count()); ++k)
						{
							auto& k_pg = tbl->at(j + k);
							if (k_pg.present())
							{
								found = false;
							}
						}
						
						if (found)
						{
							for (int k = 0; k < continuous_count; ++k)
							{
								auto& kpg = tbl->at(j+k);
								if (!kpg.allocate(!readonly(), supervisor()))
								{
									assert(false);
								}
							}
							
							return pg.virtual_address();
						}
					}
				}
				
			}
		}
		
		//TRACE("No partially-full tables.");
		assert(empty_index != -1);
		
		if (empty_index != -1)
		{
			assert(empty_index > 0);
			auto tbl = _dir->table(empty_index, true);
			assert(tbl);
			for (int j = 0; j < tbl->page_count(); ++j)
			{
				auto& pg = tbl->at(j);
				if (!pg.present())
				{
					bool found = true;
					for (int k = 1; k < continuous_count && found && (j+k < tbl->page_count()); ++k)
					{
						auto& k_pg = tbl->at(j + k);
						if (k_pg.present())
						{
							found = false;
						}
					}
					
					if (found)
					{
						for (int k = 0; k < continuous_count; ++k)
						{
							auto& kpg = tbl->at(j+k);
							if (!kpg.allocate(!readonly(), supervisor()))
							{
								assert(false);
							}
						}
						
						//std::cout << "New Table starting at " << (void*)pg.virtual_address() << std::endl;
						
						
						return pg.virtual_address();
					}
				}
			}
		}
		
		
		assert(NOT_IMPLEMENTED);
	}
	
	
	template <class MA>
	void UnifyHeap<MA>::free_page(void* addr)
	{
		assert(false);
		if ((addr_t)addr % pageSize() != 0)
		{
			addr = (void*)(((addr_t)addr / pageSize())*pageSize());
		}
		
		auto pg = _dir->at(addr);
		if (pg)
		{
			pg->present(false);
			pg->frame(nullptr);
			pg->flush();
		}
	}
}

#endif