#include <Common.h>
#include "new.hh"
#include <kernel/Memory/kheap.hh>

void* operator new(size_t size)
{
    return ::kmalloc(size, 0, 0x0);
}

void* operator new[](size_t size)
{
    return ::kmalloc(size, 0, 0x0);
}

void operator delete(void* ptr)
{
    ::free(ptr);
}

void operator delete(void* ptr, size_t)
{
    ::free(ptr);
}

void operator delete[](void* ptr)
{
    ::free(ptr);
}