#include <kernel/Utility.hh>
#include <cstdlib>
#include <iostream>
#include <kernel/Task.hh>
#include <setjmp.h>
#include <signal.h>

#define PADDR(X) std::cout << "&"#X": " << &X << std::endl

#define P(X) std::cout << #X": " << (void*)X << std::endl

#define SSIZE (sizeof(uint16_t)*2000)

int z = 0;
addr_t init_sp;

void foo();
void bar();

void* copy_stack(const void* init_sp, void*& eip);

extern "C" void* get_eip();

int main()
{
	ASM_READ_ESP(init_sp);
	//foo();
	void* ip;
	auto nstack = copy_stack((void*)init_sp, ip);
	
	std::cout << "Done!" << std::endl;
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

static addr_t tmp_sp;

void* copy_stack(const void* init_sp, void*& eip)
{
	auto tsk = new Kernel::Task;
	addr_t esp_start;
	addr_t ebp_start;
	size_t size = 0;
	void* nst = nullptr;
	addr_t offset = 0;
	
	ASM_READ_ESP(esp_start);
	ASM_GET(fp, ebp_start);
	
	
	size = esp_start - (addr_t)init_sp + 0x100;
	nst = malloc(size);
	
	memcpy(nst, (const void*)esp_start, size);
	
	offset = (addr_t)nst - esp_start;
	
	for (addr_t* p = (addr_t*)nst; p != (void*)((addr_t)nst + size); ++p)
	{
		auto tmp = *p;
		if (tmp >= esp_start && tmp <= (addr_t)init_sp)
		{
			*p = tmp + offset;
		}
	}
	
	ASM_READ_ESP(tmp_sp);
	assert(tmp_sp == esp_start);
	ASM_GET(fp, tmp_sp);
	assert(tmp_sp == ebp_start);
	get_eip();
	
	esp_start += offset;
	ebp_start += offset;
	asm volatile("mov sp, %0;\
	              mov fp, %1" : : "r"(esp_start), "r"(ebp_start));
}

void sigtest(int i)
{
	std::cout << "Sigtest " << i << std::endl;
}

extern "C"
void* get_eip()
{
	const auto n = SIGUSR2;
	struct sigaction act;
	act.sa_handler = sigtest;
	act.sa_flags |= SA_NODEFER;
	struct sigaction act_old;
	assert(sigaction(n, &act, &act_old) == 0);
	assert(raise(n) == 0);
	jmp_buf b;
	switch (setjmp(b))
	{
		case 1:
		std::cout << "Jmped!" << std::endl;
		return nullptr;
		
		default:
		std::cout << "No jmp!" << std::endl;
		break;
	}
	
	
	//longjmp(b, 1);
}
