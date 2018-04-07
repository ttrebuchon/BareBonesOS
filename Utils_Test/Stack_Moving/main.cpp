#include <kernel/Utility.hh>
#include <cstdlib>
#include <iostream>

#define PADDR(X) std::cout << "&"#X": " << &X << std::endl

#define P(X) std::cout << #X": " << (void*)X << std::endl

#define SSIZE (sizeof(uint16_t)*2000)

int z = 0;
addr_t init_sp;

void foo();
void bar();

int main()
{
	ASM_READ_ESP(init_sp);
	foo();
}

void bar()
{
	P(init_sp);
	addr_t sp;
	addr_t fp;
	ASM_READ_ESP(sp);
	P(sp);
	bool b = true;
	int x = 4;
	int y = 9;
	
	addr_t* newSPtr = (addr_t*)malloc(SSIZE);
	P(newSPtr);
	P(((addr_t)newSPtr+SSIZE));
	addr_t newsp = (addr_t)newSPtr;
	newsp += (SSIZE - (init_sp - sp));
	addr_t offset = init_sp - newsp;
	
	P(newsp);
	
	memcpy((void*)(newSPtr), (void*)(init_sp - SSIZE), SSIZE);
	
	for (int i = 0; i < SSIZE; ++i)
	{
		if (newSPtr[i] < init_sp && newSPtr[i] > (init_sp - SSIZE))
		{
			newSPtr[i] += offset;
		}
	}
	P(newsp);
	ASM_READ_ESP(sp);
	P(sp);
	asm volatile ("mov sp, %0" : : "r"(newsp));
	ASM_READ_ESP(sp);
	ASM_GET(fp, fp);
	P(sp);
	P(newsp);
	assert(sp == newsp);
	P(fp);/*
	return;
	*/
}


void foo()
{
	addr_t sp;
	char dat[1000];
	for (int i = 0; i < 1000; ++i)
	{
		dat[i] = (char)(i % 256);
	}
	
	bar();
	ASM_GET(fp, sp);
	P(sp);
	
	
	for (int i = 0; i < 1000; ++i)
	{
		assert(dat[i] == (char)(i % 256));
	}
	ASM_READ_ESP(sp);
	P(sp);
}
