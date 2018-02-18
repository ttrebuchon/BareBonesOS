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
	#ifdef FIXED_PAGE_STRUCT
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t user : 1;
	uint32_t reserved : 2;
	uint32_t accessed : 1;
	uint32_t dirty : 1;
	uint32_t reserved2 : 2;
	uint32_t avail : 3;
	uint32_t frame : 20;
	#else
	// uint32_t present : 1;
	// uint32_t rw : 1;
	// uint32_t user : 1;
	// uint32_t accessed : 1;
	// uint32_t dirty : 1;
	// uint32_t unused : 7;
	// uint32_t frame : 20;
	#endif
};

struct PageTable
{
	struct Page pages[1024];
};

struct PageDir_Entry
{
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t user : 1;
	uint32_t w_through : 1;
	uint32_t cache : 1;
	uint32_t access : 1;
	uint32_t reserved : 1;
	uint32_t page_size : 1;
	uint32_t global : 1;
	uint32_t avail : 3;
	uint32_t frame : 20;
} __attribute__((packed));

struct PageDir
{
	struct PageDir_Entry tables[1024];
	struct PageTable* ref_tables[1024];
};

extern struct PageDir* kernel_dir;
extern struct PageDir* current_dir;

void init_paging();
void switch_page_dir(struct PageDir*);
struct Page* get_page(uint32_t addr, int create, struct PageDir*);
struct PageDir* clone_page_dir(struct PageDir*);
void page_fault(Registers_t);

void init_frame_collection(uint32_t frames);
int first_frame(uint32_t*);
int first_frame_from(uint32_t*, uint32_t);
void set_frame(uint32_t);
void clear_frame(uint32_t);
uint32_t check_frame(uint32_t);
void alloc_frame(struct Page* page, int is_kernel, int is_writeable);
void free_frame(struct Page* page);


#ifdef __cplusplus
}
#endif


#endif