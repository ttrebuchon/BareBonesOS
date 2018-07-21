#ifndef INCLUDED_DRIVERS_DISK_MBRPARTITION_HH
#define INCLUDED_DRIVERS_DISK_MBRPARTITION_HH

#include "Partition.hh"
#include <kernel/Filesystem/MBR.hh>

namespace Drivers
{
	class MBRPartition : public Partition
	{
		public:
		
		protected:
		Kernel::FS::MBR_Partition_Entry_t _mbr;
		uint8_t _index;
		
		MBRPartition(Disk&, const Kernel::FS::MBR_Partition_Entry_t&, uint8_t index) noexcept;
		
		public:
		
		
		virtual size_t start() const noexcept override;
		virtual size_t end() const noexcept override;
		virtual size_t capacity() const noexcept override;
		
		static MBRPartition* Get(Disk* dev, uint8_t index);
		
		template <class Alloc>
		static MBRPartition* Get(Disk* dev, uint8_t index, const Alloc& alloc = Alloc())
		{
			if (!dev)
			{
				return nullptr;
			}
			
			assert(index < 4);
			if (index >= 4)
			{
				return nullptr;
			}
			
			auto mbr = Kernel::FS::MBR_ptable_read(dev);
			if (mbr.partitions[index].system_ID == 0)
			{
				return nullptr;
			}
			
			typename Alloc::template rebind<MBRPartition>::other palloc(alloc);
			auto part = palloc.allocate(1);
			new (part) MBRPartition(*dev, mbr.partitions[index], index);
			return part;
		}
	};
}

#endif