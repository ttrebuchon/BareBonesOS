#include "Task.hh"
#include "Memory/Paging.h"
#include "Memory/memcpy.h"

static_assert(sizeof(void*) == sizeof(addr_t));

#define EIP_MAGIC 0x12345

using Kernel::Task;

uint32_t init_esp;
Task* Task::current_task = nullptr;
uint32_t next_pid = 1;


extern uint32_t read_eip();

static void move_stack(void* new_addr, uint32_t size)
{
	addr_t i;
	for (i = (addr_t)new_addr; i >= ((addr_t)new_addr) - size; i -= 0x1000)
	{
		alloc_frame(get_page(i, 1, current_dir), 0, 1);
	}
	
	uint32_t pd_addr;
	asm volatile ("mov %%cr3, %0" : "=r"(pd_addr));
	asm volatile ("mov %0, %%cr3" : : "r"(pd_addr));
	
	
	
	addr_t oldSP, oldBP;
	asm volatile ("mov %%esp, %0" : "=r"(oldSP));
	asm volatile ("mov %%ebp, %0" : "=r"(oldBP));
	
	addr_t offset = (addr_t)new_addr - init_esp;
	
	addr_t newSP, newBP;
	newSP = oldSP + offset;
	newBP = oldBP + offset;
	
	::memcpy((void*)newSP, (void*)oldSP, init_esp-oldSP);
	
	for (i = (addr_t)new_addr; i > (addr_t)new_addr - size; i -= sizeof(addr_t))
	{
		addr_t tmp = *(addr_t*)i;
		
		
		if ((oldSP < tmp) && (tmp < init_esp))
		{
			tmp = tmp + offset;
			auto tmp2 = (addr_t*)i;
			*tmp2 = tmp;
		}
	}
	
	
	asm volatile ("mov %0, %%esp" : : "r"(newSP));
	asm volatile ("mov %0, %%ebp" : : "r"(newBP));
}










void init_tasking()
{
	asm volatile ("cli");
	
	move_stack((void*)0xE0000000, 0x2000);
	
	Task::current_task = new Task;
	
	Task::current_task->id = next_pid++;
	Task::current_task->ebp = Task::current_task->esp = 0;
	Task::current_task->instr_ptr = 0;
	Task::current_task->page_dir = current_dir;
	
	asm volatile ("sti");
}

int fork()
{
	asm volatile ("cli");
	
	
	Task* caller = Task::current_task;
	struct PageDir* new_dir = clone_page_dir(current_dir);
	
	Task* newT = new Task;
	newT->id = next_pid++;
	newT->page_dir = new_dir;
	
	uint32_t eip = read_eip();
	
	if (Task::current_task == caller)
	{
		addr_t esp, ebp;
		asm volatile ("mov %%esp, %0" : "=r"(esp));
		asm volatile ("mov %%ebp, %0" : "=r"(ebp));
		newT->esp = esp;
		newT->ebp = ebp;
		newT->instr_ptr = eip;
		
		asm volatile ("sti");
		
		return newT->id;
	}
	else
	{
		return 0;
	}
}


void task_switch()
{
	if (!Task::current_task) return;
	
	addr_t esp, ebp, eip;
	asm volatile ("mov %%esp, %0" : "=r"(esp));
	asm volatile ("mov %%ebp, %0" : "=r"(ebp));
	
	eip = read_eip();
	if (eip == EIP_MAGIC) return;
	
	
	Task::current_task->esp = esp;
	Task::current_task->ebp = ebp;
	Task::current_task->instr_ptr = eip;
	
	
	
}