#include "MBR.hh"
#include <drivers/Disk/Disk.hh>

namespace Kernel::FS
{
	static_assert(sizeof(MBR_t) == 64);
	
	int read_MBR(Drivers::Disk* disk, MBR_t* mbr)
	{
		assert(disk);
		assert(mbr);
		
		auto mbr_res = disk->read(MBR_DISK_OFFSET, sizeof(MBR_t), (uint8_t*)mbr);
		assert(mbr_res == sizeof(MBR_t));
		
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