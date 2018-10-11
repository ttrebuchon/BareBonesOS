#include "FAT32.h"
#include <drivers/Disk/Disk.hh>

#define FAT32_CLUSTER_NUM_OFF 0
#define FAT32_MAX_DIR_ENTS_PER_SEC 16
#define FAT32_DIR_ENT_SIZE 32

namespace Kernel { namespace FS
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
		
		
		assert(fs.boot.table_size_32 == sizeof(FAT32Table_t));
		
		
		assert(fs.cluster_count > 0);
		auto table_sectors = ((fs.cluster_count * 4) / sector_size()) + 1;
		fs.table_entry_count = table_sectors*sector_size() / sizeof(uint32_t);
		fs.table = new uint32_t[fs.table_entry_count];
		
		
		res = disk->read(sector_size()*fs.first_table_sector, table_sectors*sector_size(), (unsigned char*)fs.table);
		if (res != table_sectors*sector_size())
		{
			
			std::cout << sector_size()*fs.first_table_sector << "   " << res << " / " << table_sectors*sector_size() << std::endl;
		}
		
		assert(res == table_sectors*sector_size());
		// TODO
	}
	
	
	uint16_t FAT32::sectors_per_cluster() const noexcept
	{
		return fs.sectors_per_cluster;
	}
	
	uint16_t FAT32::sector_size() const noexcept
	{
		return fs.boot.bytes_per_sector;
	}
	
	
	addr_t FAT32::cluster_lba(uint32_t cluster) const noexcept
	{
		assert(cluster >= FAT32_CLUSTER_NUM_OFF);
		return fs.cluster_lba_begin + ((cluster - FAT32_CLUSTER_NUM_OFF)*fs.sectors_per_cluster);
	}
	
	int FAT32::read_cluster_sector(unsigned char* buf, uint32_t cluster, uint32_t sector) const noexcept
	{
		assert(buf);
		addr_t start = (cluster_lba(cluster) + sector)*sector_size();
		assert(start < disk->capacity());
		assert(start + sector_size() < disk->capacity());
		return disk->read(start, sector_size(), buf);
	}
	
	int FAT32::read_cluster(void* buf, uint32_t cluster) const noexcept
	{
		assert(buf);
		addr_t start = cluster_lba(cluster)*sector_size();
		assert(start < disk->capacity());
		assert(start + cluster_size() < disk->capacity());
		return disk->read(start, cluster_size(), (unsigned char*)buf);
	}
	
	
	FAT32ClusterNumber_t FAT32::next_cluster(uint32_t active) const
	{
		unsigned char FAT_table[sector_size()];
		
		uint32_t FAT_offset = active * fs.sectors_per_cluster;
		
		uint32_t FAT_sector = fs.cluster_lba_begin + (FAT_offset / sector_size());
		
		int res = disk->read(FAT_sector*sector_size(), sector_size(), FAT_table);
		assert(res == sector_size());
		
		FAT32ClusterNumber_t next;
		next.value = *reinterpret_cast<uint32_t*>(&FAT_table[(FAT_offset % sector_size())]);
		
		
		if (next.value >= FAT32_CLUSTER_EOF)
		{
			// EOF
			next.value = 0;
			next.eof = true;
		}
		else if (next.value == FAT32_CLUSTER_BAD)
		{
			next.value = 0;
			next.bad_cluster = true;
		}
		
		return next;
	}
	
	FAT32ClusterNumber_t FAT32::next_cluster(const FAT32ClusterNumber_t active) const
	{
		assert(!active.bad_cluster);
		assert(!active.eof);
		
		return next_cluster(active.value);
	}
	
	int FAT32::read_directory_cluster(uint32_t number, FAT32DirClusterEntry_t* buffer, size_t buffer_count) const
	{
		assert(buffer);
		assert(buffer_count);
		
		if (buffer_count * sizeof(FAT32DirClusterEntry_t) < fs.sectors_per_cluster*sector_size())
		{
			return -1;
		}
		
		memset(buffer, 0, buffer_count*sizeof(FAT32DirClusterEntry_t));
		int res;
		for (int i = 0; i < fs.sectors_per_cluster; ++i)
		{
			res = read_cluster_sector(reinterpret_cast<unsigned char*>(buffer) + i*sector_size(), number, i);
			assert(res == sector_size());
		}
		
		size_t entry_count = fs.sectors_per_cluster*sector_size() / sizeof(FAT32DirClusterEntry_t);
		int entry_total = 0;
		
		bool last_cluster = false;
		
		for (size_t i = 0; i < entry_count; ++i)
		{
			if (buffer[i].entry_type() == FAT32_DIR_CLUS_EOD)
			{
				entry_count = i;
				last_cluster = true;
				break;
			}
		}
		
		if (!last_cluster)
		{
			auto next_num = next_cluster(number);
			if (next_num.bad_cluster)
			{
				return -2;
			}
			else
			{
				res = read_directory_cluster(next_num.value, buffer + entry_count, buffer_count - entry_count);
				if (res < 0)
				{
					return res;
				}
				else
				{
					entry_total = res;
				}
			}
		}
		
		for (size_t i = 0; i < entry_count; ++i)
		{
			if (buffer[i].entry_type() == FAT32_DIR_CLUS_NORMAL)
			{
				
			}
		}
		
		
		return entry_total + entry_count;
	}
	
	FAT32ClusterNumber_t FAT32::root_directory_cluster() const noexcept
	{
		FAT32ClusterNumber_t n;
		n.value = fs.boot.root_cluster_num;
		n.eof = n.bad_cluster = false;
		return n;
	}
	
	uint32_t FAT32::next_free_cluster(uint32_t start_index) const
	{
		for (uint32_t i = start_index; i < fs.table_entry_count; ++i)
		{
			if (fs.table[i] == 0)
			{
				return i;
			}
		}
		return (uint32_t)-1;
	}
	
	node_ptr<> FAT32::root() const noexcept
	{
		if (!_root)
		{
			// TODO: Build nodes
			assert(NOT_IMPLEMENTED);
		}
		return _root;
	}
	
	
	
	
	
	
	
	extern "C" FAT32DirClusterEntryType dir_cluster_entry_type(const FAT32DirClusterEntry_t* entry)
	{
		const unsigned char* const raw = reinterpret_cast<const unsigned char*>(entry);
		if (raw[0] == 0x00)
		{
			return FAT32_DIR_CLUS_EOD;
		}
		else if (raw[0] == 0xE5)
		{
			return FAT32_DIR_CLUS_UNUSED;
		}
		else if (raw[10] == 0x0F)
		{
			return FAT32_DIR_CLUS_LONG_NAME;
		}
		else
		{
			return FAT32_DIR_CLUS_NORMAL;
		}
	}
	
	
	
	FAT32DirClusterEntryType FAT32DirClusterEntry_t::entry_type() const
	{
		return dir_cluster_entry_type(this);
	}
	
}
}