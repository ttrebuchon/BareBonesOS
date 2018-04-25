
#include <Common.h>
#include <stdint.h>
#include <sys/mman.h>
#include <kernel/Memory.h>
//#include <iostream>
#include <assert.h>
#include <sqlite3/sqlite3.h>

extern "C" {

/*void* malloc(unsigned long size)
{
	return malloc(size);
}

void* realloc(void* ptr, size_t n)
{
	return realloc(ptr, n);
}

void* mremap(void* old_addr, size_t osize, size_t nsize, long unsigned int flags)
{
	
}

int munmap(void* ptr, size_t len)
{
	return kmunmap(ptr, len);
}

void* memset(void* ptr, int byte, size_t size)
{
	for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
	{
		*d = byte;
	}
	return ptr;
}

void* memcpy(void*, const void*, unsigned long)
{
	
}*/

void abort()
{
	ASSERT(false);
}

void foo();


int __main()
{
	//foo();
	return 0;
	try
	{
		throw 4;
	}
	catch (...)
	{
		
	}
	
	asm volatile("svc 0x80");
	void *p1, *p2;
	static_assert(PAGE_SIZE == 4096);
	sqlite3* db;
	sqlite3_open("Test", &db);
	//std::cout << "Hello?" << std::endl;
	return 0;
}

}

//__STDC_HOSTED__
#ifdef __NOSTDLIB__
extern "C" {

void __start()
{
	__main();
	//__builtin_exit(0);
	while (true);
}
}
#endif