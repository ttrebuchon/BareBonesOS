#include "QA.hh"
#include "Out.hh"
#include <QUtils/Output/MultiStream.h>
#include <fstream>
#include <boot/MultiBoot.hh>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <fcntl.h>


//static QA::_ostream _out;

std::ostream QA::out(nullptr);
void* QA::phys_start = nullptr;
void* QA::phys_end = nullptr;


static QUtils::Output::MultiBuf* mb = nullptr;
static std::ofstream* file = nullptr;

void QA::Out_Init()
{
	if (mb)
	{
		return;
	}
	mb = new QUtils::Output::MultiBuf();
	file = new std::ofstream("QA_Out.txt");
	mb->push(file->rdbuf());
	mb->push(std::cout.rdbuf());
	out.rdbuf(mb);
	//std::clog.rdbuf(nullptr);
	//std::cout.rdbuf(nullptr);
	//out.rdbuf(std::cerr.rdbuf());
	//std::cerr.rdbuf(nullptr);
}

void QA::MultiBoot_Init()
{
	struct sysinfo sinfo;
	assert(::sysinfo(&sinfo) == 0);
	
	auto free_ram = sinfo.freehigh;
	
	auto pgs = free_ram / PAGE_SIZE;
	
	auto dzero = ::open("/dev/zero", O_RDWR | O_APPEND);
	uint32_t addrStart = 0xF0000000;
	uint32_t addrEnd = 0xFFFFFFFF;
	size_t len = addrEnd - addrStart;
	auto phys = ::mmap((void*)addrStart, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, dzero, 0);
	assert(phys != MAP_FAILED);
	
	phys_start = phys;
	phys_end = (void*)((addr_t)phys + len);
	
	
	auto mb = (multiboot*)malloc(sizeof(multiboot));
	mb->flags |= (1 << 6);
	
	auto map = (multiboot_mmap_t*)::mmap((void*)0xE0000000, 2*sizeof(multiboot_mmap_t), PROT_READ | PROT_WRITE, MAP_PRIVATE, dzero, 0);
	assert(map != MAP_FAILED);
	//auto map = (multiboot_mmap_t*)malloc(sizeof(multiboot_mmap_t));
	
	map->size = sizeof(multiboot_mmap_t);
	map->base_addr = 0x0;
	map->len = (uint64_t)(void*)phys_start;
	map->type = 0;
	
	auto map2 = &map[1];
	map2->size = sizeof(multiboot_mmap_t);
	map2->base_addr = (uint64_t)(void*)phys_start;
	map2->len = len;
	map2->type = 1;
	
	mb->mmap_addr = (uint32_t)(uint64_t)(void*)map;
	
	assert((void*)(uint64_t)mb->mmap_addr == map);
	mb->mmap_length = 2*sizeof(multiboot_mmap_t);
	
	boot::mboot = new boot::MultiBoot(mb);
}