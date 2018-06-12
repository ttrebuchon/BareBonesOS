#include "Tests.hh"
#include <kernel/Filesystem/FAT32/FAT32.h>

using namespace Kernel::Filesystem;

TEST(FAT32)
{
	static_assert(sizeof(FAT32Table_t) == 512);
	FAT32Table_t table;
	table[4] = 1;
	unsigned char* ptr = table + 10;
	
}