#ifndef INCLUDED_KERNEL_MEMORY_ALLOC_SPEC_H
#define INCLUDED_KERNEL_MEMORY_ALLOC_SPEC_H

#include <Common.h>

START_NS(Kernel)
START_NS(Memory)

C_CODE

typedef struct alloc_spec
{
	size_t size;
	size_t align;
	
	#ifdef __cplusplus
	alloc_spec(const size_t sz, const size_t align) : size(sz), align(align)
	{}
	
	alloc_spec() : alloc_spec(0, 0) {}
	#endif
}
alloc_spec_t;

extern size_t alloc_spec_req_size(alloc_spec_t, size_t count);



C_END

END_NS // Memory
END_NS // Kernel


#endif