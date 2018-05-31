#ifndef INCLUDED_SCHEDULING_CONTEXT_H
#define INCLUDED_SCHEDULING_CONTEXT_H

#include <Common.h>
#include "stack.h"
#include <kernel/Registers.h>

START_NS(Kernel)
C_CODE



typedef struct
{
	stack_t stack;
	void* ip;
	uint32_t flags;
	registers_t registers;
}
context_t;

//__attribute__((__returns_twice__))
int save_context(context_t*);


__attribute__((__noreturn__))
void load_destroy_context(context_t*, void(*destr)(context_t*));

__attribute__((__noreturn__))
void load_destroy_context2(context_t*, void(*destr)(context_t*));

__attribute__((__noreturn__))
void load_context(context_t*);

C_END
END_NS

#endif