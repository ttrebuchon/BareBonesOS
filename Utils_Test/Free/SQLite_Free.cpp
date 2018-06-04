
#include <Common.h>
#include <stdint.h>
#include <sys/mman.h>
#include <kernel/Memory.h>
//#include <iostream>
#include <assert.h>
#include <sqlite3/sqlite3.h>
#include <sys/syscall.h>
//#include <../CppUtilities/Deps/json/json.hpp>

extern "C" {

thread_local int xyz;

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

int ghk;

void* __emutls_get_address(struct __emutls_object*)
{
	return &ghk;
}


int __main()
{
	//foo();
	//return 0;
	/*try
	{
		throw 4;
	}
	catch (...)
	{
		
	}*/
	auto& jkl = xyz;
	xyz = 4;
	if (&ghk != &xyz)
	{
		*(int*)nullptr = 10;
	}
	return 0;
	

	void *p1, *p2;
	static_assert(PAGE_SIZE == 4096);
	sqlite3* db;
	sqlite3_open(":memory:", &db);
	//std::cout << "Hello?" << std::endl;
	return 0;
}

static unsigned cgv = SYS_getuid;

static_assert(SYS_getuid == 174);






}

//__STDC_HOSTED__
#ifdef __NOSTDLIB__
extern "C" {

void __start()
{
	__main();
	//__builtin_exit(0);
	/*asm volatile ("hlt 0");
	*((int*)nullptr) = 10;*/
	while (true);
}
}
#endif