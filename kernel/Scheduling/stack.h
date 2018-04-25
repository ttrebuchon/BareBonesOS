#ifndef INCLUDED_SCHEDULING_STACK_H
#define INCLUDED_SCHEDULING_STACK_H

#include <Common.h>

START_NS(Kernel)
C_CODE


typedef struct
{
	void* base;
	void* sp;
	void* fp;
}
stack_t;


C_END
END_NS

#endif