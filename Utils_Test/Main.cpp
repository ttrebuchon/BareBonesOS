#include "Tests.hh"
#include <iostream>
#include <kernel/MetaInfo.hh>

#define RUN(X) do { \
	std::cout << "-----------------\n" << "Running test for " << #X << "...\n-----------------\n" << std::endl; \
	Test_##X(); \
	std::cout << "-----------------\n" << #X << " testing done.\n-----------------\n\n\n\n" << "----------------------------------\n\n\n\n\n" << std::endl; \
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
	MI_Printer pr(std::clog);
	auto mi_pr = new MetaInfo::ClassPrinter<MI_Printer, void>(pr, &MI_Printer::write);
	MetaInfo::registerPrinter(mi_pr);
	
	
	RUN(List);
	RUNC(Bitset);
	RUN(Bitset);
	RUN(IDE);
	RUN(Limits);
	RUN(IOSTREAM);
	
	std::cerr << "\n\n\nAll Done!\n" << std::flush;
}