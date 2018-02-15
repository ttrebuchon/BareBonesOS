#include "Tests.hh"
#include <iostream>

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

int main()
{
	RUN(List);
	RUNC(Bitset);
	RUN(Bitset);
	RUN(IDE);
	RUN(IOSTREAM);
	
	std::cerr << "\n\n\nAll Done!\n" << std::flush;
}