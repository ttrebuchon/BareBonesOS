#ifndef INCLUDED_PAGING_H
#define INCLUDED_PAGING_H

#include <kernel/c_cpp.h>
#include <stdint.h>
#include <kernel/Registers.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Page
{
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t user : 1;
	uint32_t accessed : 1;
	uint32_t dirty : 1;
	uint32_t unused : 7;
	uint32_t frame : 20;
};

struct PageTable
{
	Page pages[1024];
};

struct PageDir
{
	PageTable* tables[1024];
	uint32_t physicalTables[1024];
	
	uint32_t physicalAddress;
};

void init_paging();
void switch_page_dir(struct PageDir*);
struct Page* get_page(uint32_t addr, int create, struct PageDir*);
void page_fault(struct Registers_t);

void init_frame_collection(uint32_t frames);
int first_frame(uint32_t*);
int first_frame_from(uint32_t*, uint32_t);
void set_frame(uint32_t);
void clear_frame(uint32_t);
uint32_t check_frame(uint32_t);

#ifdef __cplusplus
}
#endif


#endif