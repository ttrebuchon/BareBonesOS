#ifndef INCLUDED_KERNEL_MODULE_H
#define INCLUDED_KERNEL_MODULE_H

#include <Common.h>

#define MAX_MODULE_NAME_LENGTH 254

START_NS(Kernel)

#ifdef __cplusplus
namespace Memory
{
	template <class T>
	class virtual_allocator;
}
#endif

C_CODE

typedef IF_CPP_ELSE(Memory::virtual_allocator<uint8_t>*, void*) heap_alloc_t;

typedef struct mod_init_args
{
	heap_alloc_t heap_alloc;
	
}
mod_init_args_t;

typedef struct mod_info
{
	size_t setup_object_size;
	char name[MAX_MODULE_NAME_LENGTH+1];
	
	void*(*open)(void* obj, const mod_init_args_t*);
	void(*close)(void*);
}
mod_info_t;

typedef mod_info_t*(*mod_info_fn)(mod_info_t*) _NOEX;

mod_info_t* module_info(mod_info_t*) _NOEX __attribute__((__nonnull__(1)));

C_END
END_NS // Kernel

#endif