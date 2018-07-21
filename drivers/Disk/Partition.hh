#ifndef INCLUDED_DRIVERS_DISK_PARTITION_HH
#define INCLUDED_DRIVERS_DISK_PARTITION_HH

#include "Disk.hh"

namespace Drivers
{
	class Partition : public Disk
	{
		public:
		
		protected:
		Disk* _device;
		
		
		Partition(Disk&) noexcept;
		
		public:
		
		virtual int read(size_t start, size_t len, unsigned char* buf) const override;
		virtual int write(size_t start, size_t len, const unsigned char* buf) override;
		virtual size_t write(size_t start, uint8_t value, size_t len) override;
		
		virtual size_t capacity() const noexcept override;
		
		virtual size_t start() const noexcept = 0;
		virtual size_t end() const noexcept = 0;
	};
}

#endif