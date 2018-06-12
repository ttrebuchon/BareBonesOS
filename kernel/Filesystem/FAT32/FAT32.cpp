#include "FAT32.h"
#include <drivers/Disk/Disk.hh>

#define FAT32_CLUSTER_NUM_OFF 0
#define FAT32_MAX_DIR_ENTS_PER_SEC 16
#define FAT32_DIR_ENT_SIZE 32

namespace Kernel { namespace Filesystem
{
	
	FAT32::FAT32(Drivers::Disk& d, int partition) noexcept : Filesystem(), disk(&d), fs()
	{
		if (partition >= 0)
		{
			assert(partition < 256);
			fs.partition = partition;
		}
		
		unsigned char sector[512];
		
		int res = disk->read(0, 512, sector);
		assert(res == 512);
		
		fs.boot = *reinterpret_cast<FAT32Boot_t*>(sector);
		
		bool valid_part = false;
		
		switch (fs.boot.partition1_typecode)
		{
			case 0x0B:
			case 0x0C:
			case 0x0E:
			case 0x0F:
				valid_part = true;
				break;
				
			case 0x00:
				break;
			
			default:
				valid_part = (fs.boot.partition1_typecode <= 0x06);
				break;
				
		}
		
		if (valid_part)
		{
			fs.lba_begin = fs.boot.partition1_lba_begin;
		}
		else
		{
			fs.lba_begin = 0;
		}
		
		/*res = disk->read(fs.lba_begin, 512, sector);
		assert(res == 512);*/
		
		
		
		
		
		
		fs.sectors_per_cluster = fs.boot.sectors_per_cluster;
		
		if (fs.boot.total_sectors_16 == 0)
		{
			fs.sector_count = fs.boot.total_sectors_32;
		}
		else
		{
			fs.sector_count = fs.boot.total_sectors_16;
		}
		
		
		fs.table_size = (fs.boot.table_size_16 == 0) ? fs.boot.table_size_32 : fs.boot.table_size_16;
		
		auto root_dir_sectors = ((fs.boot.root_entry_count * 32) + (fs.boot.bytes_per_sector - 1))/fs.boot.bytes_per_sector;
		assert(root_dir_sectors == 0);
		
		
		fs.first_data_sector = fs.boot.reserved_sector_count + (fs.boot.table_count * fs.table_size);
		
		fs.first_table_sector = fs.boot.reserved_sector_count;
		
		fs.data_sector_count = fs.sector_count - (fs.boot.reserved_sector_count + (fs.boot.table_count * fs.table_size) + root_dir_sectors);
		
		fs.cluster_count = fs.data_sector_count / fs.sectors_per_cluster;
		
		
		fs.FAT_lba_begin = fs.lba_begin + fs.boot.reserved_sector_count;
		
		fs.cluster_lba_begin = fs.FAT_lba_begin + (fs.boot.table_count * fs.table_size);
		
		auto root_dir_sectors2 = ((fs.boot.root_entry_count * 32) + (fs.boot.bytes_per_sector - 1)) / fs.boot.bytes_per_sector;
		assert(root_dir_sectors == root_dir_sectors2);
		
		
		res = disk->read(512*fs.first_table_sector, 512, sector);
		assert(res == 512);
		
		
	}
	
	
	uint16_t FAT32::sector_size() const noexcept
	{
		return fs.boot.bytes_per_sector;
	}
	
	
	addr_t FAT32::cluster_lba(uint32_t cluster) const noexcept
	{
		return fs.cluster_lba_begin + ((cluster - FAT32_CLUSTER_NUM_OFF)*fs.sectors_per_cluster);
	}
	
	int FAT32::read_cluster_sector(unsigned char* buf, uint32_t cluster, uint32_t sector) const noexcept
	{
		assert(buf);
		return disk->read(cluster_lba(cluster) + sector, 512, buf);
	}
	
}
}