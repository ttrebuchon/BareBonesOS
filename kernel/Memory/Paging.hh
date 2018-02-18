#ifndef INCLUDED_PAGING_H
#define INCLUDED_PAGING_H

#include <kernel/c_cpp.h>
#include <stdint.h>
#include <kernel/Registers.h>
#include "PageDir.hh"



namespace Kernel { namespace Memory {


	extern PageDir* kernel_dir;
	extern PageDir* current_dir;

	void init_paging();
	void switch_page_dir(PageDir*);
	void page_fault(Registers_t);
	PageDir* create_empty_page_dir(uint32_t* phys);
	void* virtual_to_physical(const PageDir* dir, const void* virt_addr);

	void init_frame_collection(uint32_t frames);
	int first_frame(uint32_t*);
	int first_frame_from(uint32_t*, uint32_t);
	void set_frame(uint32_t);
	void clear_frame(uint32_t);
	uint32_t check_frame(uint32_t);

}
}


#endif