#include "Task.hh"
#include "Memory/Paging.hh"
#include "Memory/memcpy.h"
#include <kernel/utils/List.hh>
#include <kernel/Debug.h>
#include <drivers/VGA.hh>
#include "Memory/kheap.hh"

static_assert(sizeof(void*) == sizeof(addr_t));

#define EIP_MAGIC 0x12345
uint32_t init_esp;

namespace Kernel {


static void add_to_queue(Task*);

Task* Task::current_task = nullptr;
uint32_t next_pid = 1;
Utils::List<Task*>* tasks = nullptr;
Utils::List<Task*>::iterator task_iterator;


extern "C" uint32_t read_eip();

static void move_stack(void* new_addr, uint32_t size)
{
	Drivers::VGA::Write("Heap End Address: ");
	Drivers::VGA::Write((void*)Memory::kheap->endAddr());
	Drivers::VGA::Write("\n");
	addr_t i;
	for (i = (addr_t)new_addr; i >= ((addr_t)new_addr) - size; i -= 0x1000)
	{
		auto pg = Memory::current_dir->getPage(i, 1);
		ASSERT(!pg->present);
		ASSERT(pg->frame == 0);
		Memory::current_dir->getPage(i, 1)->alloc_frame(0, 1);
		auto pg2 = Memory::current_dir->getPage(i, 0);
		ASSERT(pg == pg2);
		ASSERT(pg2->present);
		ASSERT(pg2->rw);
		Drivers::VGA::Write((void*)i);
		Drivers::VGA::Write(" Paged.\n");
		// auto pg = get_page(i, 0, current_dir);
		// if (pg != 0)
		// {
		// 	ASSERT(pg->rw);
		// }
	}
	

	
	
	uint32_t pd_addr;
	asm volatile ("mov %%cr3, %0" : "=r"(pd_addr));
	asm volatile ("mov %0, %%cr3" : : "r"(pd_addr));

	((char*)new_addr)[1] = 4;

	ASSERT((addr_t)new_addr == 0xF0000000);
	ASSERT(false);
	
	
	addr_t oldSP, oldBP;
	asm volatile ("mov %%esp, %0" : "=r"(oldSP));
	asm volatile ("mov %%ebp, %0" : "=r"(oldBP));
	
	addr_t offset = (addr_t)new_addr - init_esp;
	ASSERT(init_esp != 0);
	ASSERT(offset > 0);
	
	addr_t newSP, newBP;
	newSP = oldSP + offset;
	newBP = oldBP + offset;

	auto pg = Memory::current_dir->getPage(((addr_t)new_addr) - 0x1000 + 100, 0);
	if (pg != 0)
	{
		ASSERT(pg->rw == 1);
	}


	((char*)new_addr)[-0x1000 + 100] = 4;
	//::memcpy((void*)newSP, (void*)oldSP, init_esp-oldSP);
	
	// for (int j = 0; j < (init_esp-oldSP)/sizeof(char); ++j)
	// {
	// 	((char*)newSP)[j] = ((char*)oldSP)[j];
	// }
	while (1);
	
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
	
	move_stack((void*)0xF0000000, 0x2000);
	
	Task::current_task = new Task;
	
	Task::current_task->id = next_pid++;
	Task::current_task->ebp = Task::current_task->esp = 0;
	Task::current_task->instr_ptr = 0;
	Task::current_task->page_dir = Memory::current_dir;

	tasks = new Utils::List<Task*>();
	tasks->push_back(Task::current_task);
	task_iterator = tasks->begin();
	
	asm volatile ("sti");
}

int fork()
{
	asm volatile ("cli");
	
	
	Task* caller = Task::current_task;
	Memory::PageDir* new_dir = Memory::current_dir->clone();
	
	Task* newT = new Task;
	newT->id = next_pid++;
	newT->page_dir = new_dir;

	tasks->push_back(newT);
	
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


extern "C" void task_switch()
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
	
	++task_iterator;
	if (task_iterator == tasks->end())
	{
		task_iterator = tasks->begin();
	}
	if (task_iterator == tasks->end())
	{
		return;
	}

	ASSERT(task_iterator != tasks->end());
	Task::current_task = *task_iterator;
	
	eip = Task::current_task->instr_ptr;
	esp = Task::current_task->esp;
	ebp = Task::current_task->ebp;

	Memory::current_dir = Task::current_task->page_dir;



	asm volatile("	\
	cli;	\
	mov %0, %%ecx;	\
	mov %1, %%esp;	\
	mov %2, %%ebp;	\
	mov %3, %%cr3;	\
	mov $0x12345, %%eax;	\
	sti;	\
	jmp *%%ecx	" : : "r"(eip), "r"(esp), "r"(ebp), "r"(&Memory::current_dir));
}



void add_to_queue(Task* t)
{
	tasks->push_back(t);
}

}