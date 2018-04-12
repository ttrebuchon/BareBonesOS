#ifndef INCLUDED_DUMBHEAP_HH
#define INCLUDED_DUMBHEAP_HH

#include "../Heap.hh"


namespace Kernel { namespace Memory
{
	
	class DumbHeap : public Heap
	{
		private:
		addr_t _placement;
		
		protected:
		
		
		public:
		DumbHeap(addr_t start, addr_t end, bool supervisor, bool readonly) noexcept;
		
		virtual ~DumbHeap() noexcept;

		virtual void* alloc(size_t size, size_t alignment = 0) override;
		virtual void free(void*) override;
		virtual size_t allocated_size(void*) const noexcept override;
	};
	
}
}

#endif