#ifndef INCLUDED_TASK_HH
#define INCLUDED_TASK_HH

#include <Common.h>
#include <Utils/list>
#include <Utils/shared_ptr>


class pThread;

START_NS(Kernel)

namespace Memory {
	class PageDirectory;
}

class SystemResource;

class Task
{
	public:
	static Task* volatile current_task;
	
	int id;
	addr_t esp, ebp;
	uint32_t instr_ptr;
	Memory::PageDirectory* page_dir;
	Utils::list<Utils::shared_ptr<SystemResource>> resources;
	pThread* pthread;
};

#ifdef __cplusplus
extern "C" {
#endif


void init_tasking();
void task_switch();
int fork();
int getpid();
size_t taskLength();


#ifdef __cplusplus
}
#endif
END_NS
#endif