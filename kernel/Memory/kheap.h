#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#include <kernel/c_cpp.h>
#include <stdint.h>

extern uint32_t kPlacement;

void* kmalloc(uint32_t size, int align, uint32_t* phys);

void kmemset(void*, unsigned char, uint32_t);

#endif