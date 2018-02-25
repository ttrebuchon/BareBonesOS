#ifndef INCLUDED_IDEDISK_HH
#define INCLUDED_IDEDISK_HH

#include <Common.h>
#include <drivers/Disk/Disk.hh>
#include "IDE.hh"

namespace Drivers { namespace IDE {
	
	
	class IDEDisk : public Disk
	{
		private:
		Device* dev;
		
		
		public:
		IDEDisk(Device*);
		IDEDisk(bool primary, bool master);
		
		unsigned char* readSector(const uint32_t lba) const;
		bool readSector(const uint32_t lba, unsigned char* buf) const;
		
		
		// Disk Interface
		virtual int read(size_t start, size_t len, unsigned char* buf) const override;
		virtual int write(size_t start, size_t len, const unsigned char* buf) override;
		virtual size_t capacity() const override;
		
		// Methods
		const char* model() const;
		
	};
	
	
}
}

#endif