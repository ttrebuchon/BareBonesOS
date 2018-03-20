#include "Tests.hh"
#include <iostream>
#include <kernel/MetaInfo.hh>

void checkMemoryTrack();

#define RUN(X) do { \
	std::cout << "-----------------\n" << "Running test for " << #X << "...\n-----------------\n" << std::endl; \
	Test_##X(); \
	std::cout << "-----------------\n" << #X << " testing done.\n-----------------\n\n\n\n"; \
	checkMemoryTrack(); \
	QA::Memory::Reset(); \
	std::cout << "----------------------------------\n\n\n\n\n" << std::endl; \
	} while (false)

#define RUNC(X) do { \
	std::cout << "-----------------\n" << "Running C test for " << #X << "...\n-----------------\n" << std::endl; \
	TestC_##X(); \
	std::cout << "-----------------\n" << #X << " C testing done.\n-----------------\n\n\n\n" << "----------------------------------\n\n\n\n\n" << std::endl; \
	} while (false)

class MI_Printer
{
	private:
	std::ostream& os;
	
	public:
	
	void write(const char* cstr)
	{
		os << cstr;
	}
	
	MI_Printer(std::ostream& os) : os(os)
	{}
};

int main()
{
	QA::Memory::Init();
	
	MI_Printer pr(std::clog);
	auto mi_pr = new MetaInfo::ClassPrinter<MI_Printer, void>(pr, &MI_Printer::write);
	MetaInfo::registerPrinter(mi_pr);
	
	
	RUN(IOSTREAM);
	//RUN(map);
	RUN(List);
	RUNC(Bitset);
	RUN(Bitset);
	RUN(IDE);
	RUN(Limits);
	RUN(String);
	//RUN(IOSTREAM);
	RUN(SQLite);
	RUN(shared_ptr);
	RUN(vector);
	RUN(tuple);
	//RUN(map);
	RUN(unordered_map);
	
	std::cerr << "\n\n\nAll Done!\n" << std::flush;
}



void checkMemoryTrack()
{
	#ifdef TRACK_ALLOC
	std::cout << "Total Allocated: " << QA::Memory::Total << "\n";
	std::cout << "Allocations: " << QA::Memory::Allocations.size() << "\n";
	std::cout << "Unfreed: " << QA::Memory::Map.size() << "\n";
	if (QA::Memory::Map.size() > 0)
	{
		std::cout << "\tTotalling: ";
		size_t tot = 0;
		size_t arrays = 0;
		for (const auto& p : QA::Memory::Map)
		{
			tot += p.second->size();
			arrays += p.second->array();
		}
		std::cout << tot << "\n\tWhere " << arrays << " unfreed were arrays\n";
	}
	#endif
}