#include "MetaInfo.h"
#include "MetaInfo.hh"
#include "utils/int_to_str.h"

#define PRINT_COUNT 256

using namespace MetaInfo;

static long idCounter = 1;

struct PrinterEntry
{
	long id;
	Printer* printer;
};

static PrinterEntry printers[PRINT_COUNT];
	
static int initPrinters()
{
	idCounter = 1;
	for (int i = 0; i < PRINT_COUNT; ++i)
	{
		printers[i] = {-1, nullptr};
	}
	return 0;
}

static int printCount = initPrinters();



namespace MetaInfo {

long registerPrinter(Printer* prin)
{
	if (printCount == PRINT_COUNT || prin == nullptr)
	{
		return -1;
	}
	
	printers[printCount++] = {idCounter, prin};
	return idCounter++;
}

Printer* unregisterPrinter(long id)
{
	if (id < 0)
	{
		return nullptr;
	}
	
	for (int i = 0; i < printCount; ++i)
	{
		if (printers[i].id == id)
		{
			Printer* ptr = printers[i].printer;
			for (int j = i+1; j < printCount; ++j)
			{
				printers[j-1] = printers[j];
			}
			--printCount;
			return ptr;
		}
	}
	return nullptr;
}
}

static char intArr[256];

extern "C" {

void __stack_info_func(const char* file, const int line, const char* func, const char* msg)
{
	
	c_int_to_str(line, intArr, 256);
	for (int i = 0; i < printCount; ++i)
	{
		printers[i].printer->print(file);
		printers[i].printer->print("::");
		printers[i].printer->print(intArr);
		printers[i].printer->print("::");
		printers[i].printer->print(func);
		printers[i].printer->print("\n");
	}
}

}