#ifndef INCLUDED_HEAP_HH
#define INCLUDED_HEAP_HH

#include <Common.h>

namespace Kernel { namespace Memory
{
	class Heap
	{
		private:
		addr_t _startAddr;
		addr_t _endAddr;
		bool _supervisor;
		bool _readonly;
		size_t pSize;
		
		protected:
		__attribute__((always_inline))
		virtual void setEnd(addr_t e)
		{
			_endAddr = e;
		}
		
		virtual void* _realloc(void*, size_t oldSize, size_t newSize, size_t alignment);
		

		public:
		Heap(addr_t start, addr_t end, size_t pSize, bool supervisor, bool readonly) noexcept;
		
		virtual ~Heap() noexcept = default;

		virtual void* alloc(size_t size, size_t alignment = 0) = 0;
		virtual void free(void*) = 0;
		void* realloc(void*, size_t, size_t alignment = 0);
		virtual size_t allocated_size(void*) const noexcept = 0;
		
		
		__attribute__((always_inline))
		addr_t startAddr() const
		{ return _startAddr; }
		
		__attribute__((always_inline))
		addr_t endAddr() const
		{ return _endAddr; }
		
		__attribute__((always_inline))
		bool supervisor() const
		{ return _supervisor; }
		
		__attribute__((always_inline))
		bool readonly() const
		{ return _readonly; }
    };
}
}

#endif