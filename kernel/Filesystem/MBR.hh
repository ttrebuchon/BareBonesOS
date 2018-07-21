#ifndef INCLUDED_KERNEL_FS_MBR_HH
#define INCLUDED_KERNEL_FS_MBR_HH

#include <Common.h>

#define MBR_DISK_OFFSET 0x01BE
#define MBR_ACTIVE 0x80

namespace Drivers
{
	class Disk;
}

namespace Kernel::FS
{
	// Little-endian
	struct MBR_Partition_Entry_t
	{
		uint8_t bootable;
		uint8_t start_head;
		uint16_t start_sector : 6;
		uint16_t start_cyl : 10;
		uint8_t system_ID;
		uint8_t end_head;
		uint16_t end_sector : 6;
		uint16_t end_cyl : 10;
		uint32_t relative_sector;
		uint32_t total_sectors;
		
	} __attribute__((__packed__));
	
	static_assert(sizeof(MBR_Partition_Entry_t) == 16);
	
	struct MBR_PTable_t
	{
		MBR_Partition_Entry_t partitions[4];
		
	} __attribute__((__packed__));
	
	int MBR_ptable_read(Drivers::Disk*, MBR_PTable_t*);
	MBR_PTable_t MBR_ptable_read(Drivers::Disk*);
	int MBR_ptable_write(Drivers::Disk*, const MBR_PTable_t*);
}


#endif