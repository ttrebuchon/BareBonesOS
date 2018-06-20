#ifdef TESTING
#include "Tests.hh"
#include <iostream>
#include <kernel/MetaInfo.hh>
#include <boot/multiboot.h>

#define MAC(X) TEST(X)
#define CMAC(X) CTEST(X)

#include "Tests.inc"

#undef MAC
#undef CMAC

/*TEST(List);
CTEST(Bitset);
TEST(Bitset);
TEST(IDE);
TEST(IOSTREAM);
TEST(Limits);
TEST(String);
TEST(SQLite);
TEST(shared_ptr);
TEST(vector);
TEST(tuple);
TEST(map);
TEST(unordered_map);
TEST(kernel_utility);
TEST(mutex);
TEST(function);
TEST(list_vector);
CTEST(FileDescriptors);
TEST(queue);
TEST(PhysicalMemory);
TEST(context);*/
TEST(mutex);


#ifdef __EXCEPTIONS
#include <Utils/exception>
#endif



//#define SQLITE_DB "SomeDB.sqlite"

#ifndef SQLITE_DB
#define SQLITE_DB ":memory:"
#endif


const char* sqlite_out_db = SQLITE_DB;



void checkMemoryTrack();

#define RUN(X) do { \
	QA::out << "-----------------\n" << "Running test for " << #X << "...\n-----------------\n" << std::endl; \
	QA::Memory::Reset(); \
	QA::Memory::Start(); \
	Test_##X(); \
	QA::Memory::Pause(); \
	QA::out << "-----------------\n" << #X << " testing done.\n-----------------\n\n\n\n"; \
	checkMemoryTrack(); \
	QA::out << "----------------------------------\n\n\n\n\n" << std::endl; \
	QA::CheckPIC(); \
	} while (false)

#define RUNC(X) do { \
	QA::out << "-----------------\n" << "Running C test for " << #X << "...\n-----------------\n" << std::endl; \
	TestC_##X(); \
	QA::out << "-----------------\n" << #X << " C testing done.\n-----------------\n\n\n\n" << "----------------------------------\n\n\n\n\n" << std::endl; \
	} while (false)

#define RUN_NO_TRACK_ALLOC(X) do { \
	QA::out << "-----------------\n" << "Running test for " << #X << "...\n-----------------\n" << std::endl; \
	QA::Memory::Reset(); \
	Test_##X(); \
	QA::out << "-----------------\n" << #X << " testing done.\n-----------------\n\n\n\n"; \
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
	QA::Init();
	
	MI_Printer pr(std::clog);
	auto mi_pr = new MetaInfo::ClassPrinter<MI_Printer, void>(pr, &MI_Printer::write);
	MetaInfo::registerPrinter(mi_pr);
	
	#ifdef __EXCEPTIONS
	try
	{
	#endif
	
	#define MAC(X) RUN(X)
	#define CMAC(X) RUNC(X)
	
	RUN_NO_TRACK_ALLOC(mutex);
	#include "Tests.inc"
	
	#undef MAC
	#undef CMAC
	
	
	/*RUN(context);
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
	RUN(map);
	RUN(unordered_map);
	RUN(kernel_utility);
	RUN_NO_TRACK_ALLOC(mutex);
	RUN(function);
	RUN(list_vector);
	RUNC(FileDescriptors);
	RUN(queue);
	RUN(PhysicalMemory);
	RUN(reflect);*/
	
	#ifdef __EXCEPTIONS
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		throw;
	}
	catch (Utils::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		throw;
	}
	catch (...)
	{
		throw;
	}
	#endif
	
	std::cerr << "\n\n\nAll Done!\n" << std::flush;
}



void checkMemoryTrack()
{
	#ifdef TRACK_ALLOC
	QA::out << "Total Allocated: " << QA::Memory::Total << "\n";
	QA::out << "Allocations: " << QA::Memory::Allocations.size() << "\n";
	QA::out << "Unfreed: " << QA::Memory::Map.size() << "\n";
	if (QA::Memory::Map.size() > 0)
	{
		QA::out << "\tTotalling: ";
		size_t tot = 0;
		size_t arrays = 0;
		for (const auto& p : QA::Memory::Map)
		{
			tot += p.second->size();
			arrays += p.second->array();
		}
		QA::out << tot << "\n\tWhere " << arrays << " unfreed were arrays\n";
		
		QA::out << "\n\tUnfreed:\n";
		for (const auto& p : QA::Memory::Map)
		{
			QA::out << "\t\t" << p.first << "\t-\t" << p.second->size() << "\n";
			assert(p.first == p.second->addr());
		}
	}
	QA::out << std::flush;
	#endif
}
#endif