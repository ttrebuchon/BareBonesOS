#ifndef INCLUDED_TASK_HH
#define INCLUDED_TASK_HH

#include <Common.h>

struct PageDir;

START_NS(Kernel)


class Task
{
	public:
	static Task* current_task;
	
	int id;
	addr_t esp, ebp;
	uint32_t instr_ptr;
	PageDir* page_dir;
};

#ifdef __cplusplus
extern "C" {
#endif


void init_tasking();
void task_switch();
int fork();
int getpid();



#ifdef __cplusplus
}
#endif
END_NS
#endif