#ifndef INCLUDED_KERNEL_PROCESSOR_H
#define INCLUDED_KERNEL_PROCESSOR_H

//#include <kernel/Interrupts/ISR.h>
#include <Common.h>
#include <Types.h>

#ifdef __cplusplus
namespace Kernel { namespace Memory {
	
	class PageDirectory;
}
}
#endif

START_NS(Kernel)
C_CODE

// Forward Declaration
typedef struct __thread_struct __thread_t;



// For processor specific data
// Each Processor_t will be page-mapped to the same virtual address in their kernel
// page directory mapping

typedef struct
{
	uint8_t ID;
	volatile int cli_count;
	isr_t interrupt_handlers[256];
	#ifdef __cplusplus
	Memory::PageDirectory* page_dir;
	Memory::PageDirectory* current_page_dir;
	#else
	void* page_dir;
	void* current_page_dir;
	#endif
	__thread_t* current_thread;
	
} __attribute__((__aligned__ (4096)))
Processor_t;

// The page-mapped address
extern Processor_t* current_processor;

#ifdef __cplusplus
	static_assert(sizeof(Processor_t) == 4096);
#endif

C_END
END_NS


#endif