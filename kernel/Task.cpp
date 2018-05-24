#include "Task.hh"
#include "Memory/Paging.hh"
#include "Memory/memcpy.h"
#include <Utils/List.hh>
#include <kernel/Debug.h>
#include <drivers/VGA.hh>
#include "Memory/kheap.hh"

static_assert(sizeof(void*) == sizeof(addr_t));

#define EIP_MAGIC 0x12345
uint32_t init_esp;

namespace Kernel {


static void add_to_queue(Task*);

Task* volatile Task::current_task = nullptr;
uint32_t next_pid = 1;
Utils::List<Task*>* tasks = nullptr;
Utils::List<Task*>::iterator task_iterator;

extern Memory::PageDir* kernel_dir;

extern "C" uint32_t read_eip();

extern "C"
void move_stack(void* new_addr, uint32_t size)
{
	bool result = Memory::PageDirectory::Current->map((addr_t)new_addr-size, size, true, false);
	ASSERT(result);
	TRACE("STARTING TASKING");
	
	addr_t i;


	addr_t oldSP;
	asm volatile ("mov %%esp, %0" : "=r"(oldSP));
	addr_t oldBP;
	asm volatile ("mov %%ebp, %0" : "=r"(oldBP));

	addr_t offset = (addr_t)new_addr - init_esp;

	addr_t newSP = oldSP + offset;
	addr_t newBP = oldBP + offset;
	memcpy((void*)newSP, (void*)oldSP, init_esp - oldSP);

	// Adjust stack addresses

	for (i = (addr_t)new_addr-4; i > (addr_t)new_addr - size; i -= sizeof(addr_t))
	{
		addr_t tmp = *(addr_t*)i;

		if ((oldSP < tmp) && (tmp < init_esp))
		{
			tmp += offset;
			*(addr_t*)i = tmp;
		}
	}

	asm volatile ("mov %%esp, %0" : "=r"(i));
	//ASSERT(i == oldSP);
	newSP += i - oldSP;
	asm volatile ("mov %%ebp, %0" : "=r"(i));
	ASSERT(i >= oldBP);
	newBP += i - oldBP;


	asm volatile ("mov %0, %%esp" : : "r"(newSP));
	asm volatile ("mov %0, %%ebp" : : "r"(newBP));
}








static addr_t tmp_esp = 0;

void init_tasking()
{
	asm volatile ("cli");
	Memory::kernel_dir->clone(Memory::kernel_dir)->switch_to();
	assert(Memory::PageDirectory::Current != Memory::kernel_dir);
	asm volatile("MOV %%esp, %0" : "=r"(tmp_esp));

	assert(tmp_esp < init_esp);
	move_stack((void*)0xE0000000, init_esp - tmp_esp + 0x1000);
	//move_stack((void*)0xE0000000, 0x2000);
	Drivers::VGA::Write("Stack moved.\n");
	
	Task* new_task = new Task;
	
	
	new_task->id = next_pid++;
	new_task->ebp = Task::current_task->esp = 0;
	new_task->instr_ptr = 0;
	new_task->page_dir = Memory::PageDirectory::Current;

	tasks = new Utils::List<Task*>();
	tasks->push_back(new_task);
	task_iterator = tasks->begin();
	Task::current_task = new_task;
	
	asm volatile ("sti");
}

static Task* volatile prev_caller = nullptr;

extern "C"
int fork()
{
	if (!Task::current_task) return -1;
	asm volatile ("cli");

	assert(Memory::PageDirectory::Current != Memory::kernel_dir);
	
	Task* caller = Task::current_task;
	Memory::PageDirectory* new_dir = Memory::PageDirectory::Current->clone(Memory::kernel_dir);
	
	
	
	Task* newT = new Task;
	newT->id = next_pid++;
	newT->page_dir = new_dir;
	newT->esp = newT->ebp = 0;
	newT->instr_ptr = 0;

	tasks->push_back((Task*)newT);

	addr_t esp;
	addr_t ebp;
	
	addr_t eip = read_eip();
	asm volatile("mov %%esp, %0" : "=m"(esp));
	asm volatile("mov %%ebp, %0" : "=m"(ebp));
	if (Task::current_task == caller)
	{
		// addr_t esp; asm volatile("mov %%esp, %0" : "=r"(esp));
		// addr_t ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));
		//asm volatile("mov %%esp, %0" : "=r"(esp));
		//asm volatile("mov %%ebp, %0" : "=r"(ebp));
		// assert(Task::current_task->id == 1);
		newT->esp = esp;
		newT->ebp = ebp;
		newT->instr_ptr = eip;
		// TRACE("ORIGINAL EXITING");
		asm volatile ("sti");
		return newT->id;
	}
	else
	{
		return 0;
	}
	
	
	// asm volatile ("cli");
	// TRACE("CREATED EXITING");
	// return 0;
}

// extern "C"
// int fork()
// {
// 	// Task* volatile caller = Task::current_task;
// 	// volatile int x = 0;
// 	// x = __fork();
// 	// if (x == 0)
// 	// {
// 	// 	assert(Task::current_task != caller);
// 	// 	return 0;
// 	// }
// 	// assert(Task::current_task == caller);
// 	// return x;
// 	return __fork();
// }

extern "C" void task_switch()
{
	if (!Task::current_task) return;
	if (tasks->size() == 1)
	{
		return;
	}

	addr_t esp, ebp, eip, dir_phys;
	asm volatile ("mov %%esp, %0" : "=r"(esp));
	asm volatile ("mov %%ebp, %0" : "=r"(ebp));

	

	eip = read_eip();
	if (eip == 0x12345) return;

	

	Task::current_task->instr_ptr = eip;
	Task::current_task->ebp = ebp;
	Task::current_task->esp = esp;
	
	++task_iterator;
	if (task_iterator == tasks->end())
	{
		task_iterator = tasks->begin();
	}
	Task::current_task = *task_iterator;

	eip = Task::current_task->instr_ptr;
	esp = Task::current_task->esp;
	ebp = Task::current_task->ebp;

	Memory::PageDirectory::Current = Task::current_task->page_dir;
	dir_phys = reinterpret_cast<addr_t>(Memory::PageDirectory::Current->thisPhysical());




	asm volatile("\
	cli; \
	mov %0, %%ecx; \
	mov %1, %%esp; \
	mov %2, %%ebp; \
	mov %3, %%cr3; \
	mov $0x12345, %%eax; \
	sti; \
	jmp *%%ecx" : : "r"(eip), "r"(esp), "r"(ebp), "r"(dir_phys));
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