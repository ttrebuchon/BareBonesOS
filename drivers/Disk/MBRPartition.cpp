#include "MBRPartition.hh"
#include <Utils/allocator.hh>

namespace Drivers
{
	MBRPartition::MBRPartition(Disk& d, const Kernel::FS::MBR_Partition_Entry_t& mbr, uint8_t index) noexcept : Partition(d), _mbr(mbr), _index(index)
	{
		
	}
	
	
	
	size_t MBRPartition::start() const noexcept
	{
		return size_t(_mbr.relative_sector)*512;
	}
	
	size_t MBRPartition::end() const noexcept
	{
		return (size_t(_mbr.relative_sector) + size_t(_mbr.total_sectors))*512;
	}
	
	size_t MBRPartition::capacity() const noexcept
	{
		return 512*size_t(_mbr.total_sectors);
	}
	
	
	
	
	MBRPartition* MBRPartition::Get(Disk* dev, uint8_t index)
	{
		return Get(dev, index, Utils::allocator<void>());
	}
	
}