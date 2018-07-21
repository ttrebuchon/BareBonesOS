#include "MBR.hh"
#include <drivers/Disk/Disk.hh>

namespace Kernel::FS
{
	static_assert(sizeof(MBR_PTable_t) == 64);
	
	int MBR_ptable_read(Drivers::Disk* disk, MBR_PTable_t* mbr)
	{
		assert(disk);
		assert(mbr);
		
		auto mbr_res = disk->read(MBR_DISK_OFFSET, sizeof(MBR_PTable_t), (uint8_t*)mbr);
		assert(mbr_res == sizeof(MBR_PTable_t));
		
		int count = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (mbr->partitions[i].system_ID != 0)
			{
				++count;
			}
		}
		
		return count;
	}
	
	MBR_PTable_t MBR_ptable_read(Drivers::Disk* disk)
	{
		MBR_PTable_t table;
		MBR_ptable_read(disk, &table);
		return table;
	}
	
	
	int MBR_ptable_write(Drivers::Disk* disk, const MBR_PTable_t* mbr)
	{
		assert(disk);
		assert(mbr);
		
		auto mbr_res = disk->write(MBR_DISK_OFFSET, sizeof(MBR_PTable_t), (const uint8_t*)mbr);
		assert(mbr_res == sizeof(MBR_PTable_t));
		
		int count = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (mbr->partitions[i].system_ID != 0)
			{
				++count;
			}
		}
		
		return count;
	}
	
}