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
	auto pg = Memory::current_dir->getPage((uint32_t)new_addr, 0);
	ASSERT(pg == 0x0);
	addr_t i;
	for (i = (addr_t)new_addr; i >= ((addr_t)new_addr) - size; i -= 0x1000)
	{
		pg = Memory::current_dir->getPage(i, 1);
		if (!pg->present)
		{
			pg->alloc_frame(0, 1);
		}
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
	//asm volatile ("invlpg %0" : : "r"((uint32_t)new_addr));

	//((char*)new_addr)[1] = 4;

	ASSERT((addr_t)new_addr == 0xF0000000);
	
	
	addr_t oldSP, oldBP;
	asm volatile ("mov %%esp, %0" : "=r"(oldSP));
	asm volatile ("mov %%ebp, %0" : "=r"(oldBP));
	
	addr_t offset = (addr_t)new_addr - init_esp;
	ASSERT(init_esp != 0);
	ASSERT(offset > 0);
	
	addr_t newSP, newBP;
	newSP = oldSP + offset;
	newBP = oldBP + offset;

	pg = Memory::current_dir->getPage(((addr_t)new_addr) - 0x1000 + 100, 0);
	ASSERT(pg != 0);
	ASSERT(pg->rw == 1);


	pg = Memory::current_dir->getPage((uint32_t)newSP, 0);
	ASSERT(pg != 0);
	ASSERT(pg->rw);
	ASSERT(pg->present);
	// ::memcpy((void*)newSP, (void*)oldSP, init_esp-oldSP);
	TRACE("Copying stack...\n");
	for (unsigned int j = 1; j < (init_esp-oldSP)/sizeof(unsigned char); ++j)
	{
		// pg = Memory::current_dir->getPage(oldSP, 0);
		// ASSERT(pg != 0);
		// ASSERT(pg != nullptr);
		// ASSERT(pg->present);
		// pg->rw = true;
		// pg = Memory::current_dir->getPage(oldSP, 0);
		// ASSERT(pg != 0);
		// ASSERT(pg->rw);
		// asm volatile ("mov %%cr3, %0" : "=r"(pd_addr));
		// asm volatile ("mov %0, %%cr3" : : "r"(pd_addr));
		
		
		Drivers::VGA::Write("Address: ");
		Drivers::VGA::Write(virtual_to_physical(Memory::current_dir, (void*)oldSP));
		Drivers::VGA::Write("\n");
		//ASSERT(false);
		unsigned char byte = *(((unsigned char*)oldSP) + j);
		//ASSERT(false);
		TRACE("byte read.\n");
		pg = Memory::current_dir->getPage((uint32_t)(((unsigned char*)newSP)+j), 0);
		ASSERT(pg != 0);
		ASSERT(pg->present);
		ASSERT(pg->rw);
		ASSERT(pg->frame != 0);
		ASSERT(pg->user);
		ASSERT(pg->reserved == 0);
		ASSERT(pg->reserved2 == 0);
		unsigned char byte2 = *(((unsigned char*)newSP)+j);
		pg->accessed = true;
		ASSERT(pg->accessed);
		TRACE("Accessed set to true.\n");
		*(((unsigned char*)newSP)+j) = byte;
		//((unsigned char*)newSP)[j] = byte;
		TRACE("byte copied.\n");
		ASSERT(false);
	}
	TRACE("Stack copied.\n");
	
	Memory::switch_page_dir(Memory::current_dir);
	
	for (i = (addr_t)new_addr; i > (addr_t)new_addr - size; i -= sizeof(addr_t))
	{
		addr_t tmp = *(addr_t*)i;
		TRACE("Data read...\n");
		
		
		if ((oldSP < tmp) && (tmp < init_esp))
		{
			tmp = tmp + offset;
			auto tmp2 = (addr_t*)i;
			*tmp2 = tmp;
		}
	}
	while (1);
	
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
	if (!Task::current_task) return -1;
	
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
	if (eip == EIP_MAGIC)
	{
		//DEBUG
		TRACE("Task rezzed.\n");
		while (1);
		return;
	}
	
	
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
	if ((*task_iterator)->page_dir == Task::current_task->page_dir)
	{
		uint32_t _cr3;
		asm volatile ("mov %%cr3, %0" : "=r"(_cr3));
		ASSERT((void*)_cr3 == virtual_to_physical(Memory::current_dir, (*task_iterator)->page_dir));
		//ASSERT(false);
	}
	//ASSERT(false);
	Task::current_task = *task_iterator;
	
	eip = Task::current_task->instr_ptr;
	esp = Task::current_task->esp;
	ebp = Task::current_task->ebp;

	

	uint32_t dir_phys = (uint32_t)virtual_to_physical(Memory::current_dir, &Task::current_task->page_dir->tables);
	ASSERT(dir_phys != 0);
	ASSERT((void*)&Task::current_task->page_dir->tables == Task::current_task->page_dir->tables);

	Memory::current_dir = Task::current_task->page_dir;

	asm volatile("	\
	cli;	\
	mov %0, %%ecx;	\
	mov %1, %%esp;	\
	mov %2, %%ebp;	\
	mov %3, %%cr3;	\
	mov $0x12345, %%eax;	\
	sti;	\
	jmp *%%ecx	" : : "r"(eip), "r"(esp), "r"(ebp), "r"(dir_phys));
	
	//Should never reach here
	while (1);
}



void add_to_queue(Task* t)
{
	tasks->push_back(t);
}

size_t taskLength()
{
	if (tasks)
	{
		return tasks->size();
	}
	return 0;
}

}