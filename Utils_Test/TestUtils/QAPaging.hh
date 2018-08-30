#pragma once
#include <map>
#include <typeinfo>
#include <list>
#include <string>
#include "QA.hh"


#define QA_PHYSICAL_MEMORY_LEN 0x3000000


struct QAMapping
{
	uintptr_t physical : (sizeof(uintptr_t)*8-4);
	uintptr_t linear : (sizeof(uintptr_t)*8-4);
	bool present : 1;
};



class QAPageConfig
{
	public:
	
	private:
	QAMapping mappings[QA_PHYSICAL_MEMORY_LEN/0x1000];
	
	public:
	
	bool load();
};

class QA::Paging final
{
	public:
	constexpr static const char* filename = "Physical_Memory.bin";
	constexpr static size_t physical_size = QA_PHYSICAL_MEMORY_LEN;
	//constexpr static uintptr_t physical_start = 0xD0000000;
	
	private:
	static int phys_fd;
	static QAPageConfig* config;
	
	public:
	
	static bool map(void* linear, void* phys) noexcept;
	
	static bool map(void* linear, void* phys, size_t length) noexcept;
	
	static bool unmap(void* linear) noexcept;
	
	public:
	
	
	
	static void Initialize() noexcept;
	
	friend class QAPageConfig;
};



		