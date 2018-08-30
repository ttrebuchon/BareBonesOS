#include "Tests.hh"
#include <kernel/Filesystem/FAT32/FAT32.h>
#include <drivers/Disk/Disk.hh>

using namespace Kernel::FS;



TEST(FAT32)
{
	static_assert(sizeof(FAT32Table_t) == 512);
	FAT32Table_t table;
	table[4] = 1;
	unsigned char* ptr = table + 10;
	
	const uint32_t disk_sz = 20480;
	
	auto drive = QA::QADrive("Images/FAT32Disk.img", disk_sz);
	assert(drive);
	
	unsigned char sector[512];
	drive->read(0, 512, sector);
	
	FAT32Boot_t boot = *reinterpret_cast<FAT32Boot_t*>(sector);
	
	assert(boot.partition1_typecode == 0);
	boot.sectors_per_cluster = 2;
	boot.bytes_per_sector = 512;
	boot.total_sectors_32 = drive->capacity() / 512;
	boot.table_size_32 = 512;
	boot.root_cluster_num = 2;
	
	
	memcpy(sector, &boot, sizeof(boot));
	drive->write(0, 512, sector);
	
	
	{
		FAT32 fat(*drive);
		assert(fat.sector_size() == 512);
		
		memset(sector, 1, 512);
		
		int res = fat.read_cluster_sector(sector, 1, 0);
		assert(res == 512);
		
		for (int i = 0; i < 512; ++i)
		{
			assert(sector[i] == 0);
		}
		
		FAT32DirClusterEntry_t root_ents[64];
		
		auto rc = fat.read_directory_cluster(fat.root_directory_cluster().value, root_ents, 64);
		assert(rc == 0);
		
		auto ind = fat.next_free_cluster();
		
		/*auto n = fat.getNode("/SomeFile.txt");
		assert(n);*/
	}
	
	
	
	
	delete drive;
}

