#pragma once
#include <map>
#include <typeinfo>
#include <list>
#include <string>
#include "QA.hh"

#ifdef __USE_MEM_POOL__
#define QA_PHYSICAL_MEMORY_LEN (0x200000000 - 1)
#else
#define QA_PHYSICAL_MEMORY_LEN 0x3000000
#endif

#define CEIL(X, V) ( ( (X) / (V) ) + ( ( (X) % (V) ) != 0 ? 1 : 0 ) )
#define CEIL_REM(X, Y) ( (Y) * (CEIL(X, Y)) - (X) )


struct QAMapping
{
	uintptr_t physical;// : (sizeof(uintptr_t)*8-4);
	uintptr_t linear;// : (sizeof(uintptr_t)*8-4);
	bool present;// : 1;
	uint32_t page_len;// : (24 + CEIL_REM((2*(8*sizeof(uintptr_t) - 4) + 1), 8));
} __attribute__((__packed__));

#undef CEIL_REM
#undef CEIL



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
	constexpr static const char* filename = "Tmp/Physical_Memory.bin";
	constexpr static size_t physical_size = QA_PHYSICAL_MEMORY_LEN;
	//constexpr static uintptr_t physical_start = 0xD0000000;
	
	private:
	static int phys_fd;
	static QAPageConfig* config;
	
	public:
	
	static bool map(const void* linear, const void* phys) noexcept;
	
	static bool map(void* linear, void* phys, size_t length) noexcept;
	
	static bool unmap(void* linear) noexcept;
	
	public:
	
	
	
	static void Initialize() noexcept;
	
	friend class QAPageConfig;
};



		