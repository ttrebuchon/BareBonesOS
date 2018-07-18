#ifndef INCLUDED_KERNEL_FILESYSTEM_DEVICEFILEADAPTER_HH
#define INCLUDED_KERNEL_FILESYSTEM_DEVICEFILEADAPTER_HH

#include "FileNode.hh"
#include <drivers/Disk/Disk.hh>

namespace Kernel::FS
{
	class DeviceFileAdapter : public Drivers::Disk
	{
		public:
		
		protected:
		FileNode_v* file;
		
		public:
		DeviceFileAdapter(FileNode_v* file);
		
		virtual int read(size_t start, size_t len, unsigned char* buf) const override;
		virtual int write(size_t start, size_t len, const unsigned char* buf) override;
		
		virtual size_t capacity() const override;
	};
}

#endif