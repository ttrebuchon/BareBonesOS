#include <Common.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

namespace Kernel::Memory
{
	
	
	void* allocate_cross_processor(size_t sz, size_t alignment)
	{
		void* ptr;
		if (alignment < sizeof(void*))
		{
			alignment = sizeof(void*);
		}
		int res = posix_memalign(&ptr, alignment, sz);
		if (res != 0)
		{
			std::cout << strerror(res) << std::endl;
			std::cout << "[" << sz << ", " << alignment << "]" << std::endl;
			assert(false);
		}
		assert((size_t)ptr % alignment == 0);
		return ptr;
	}
	
	void free_cross_processor(void* ptr)
	{
		free(ptr);
	}
	
}