#include "DeviceFileAdapter.hh"


namespace Kernel::FS
{
	using namespace Drivers;
	DeviceFileAdapter::DeviceFileAdapter(FileNode_v* file) : Disk(), file(file)
	{
		assert(file);
		if (!file)
		{
			// TODO: Error out
		}
	}
	
	
	int DeviceFileAdapter::read(size_t start, size_t len, unsigned char* buf) const
	{
		return file->read(start, len, buf);
	}
	
	int DeviceFileAdapter::write(size_t start, size_t len, const unsigned char* buf)
	{
		return file->write(start, len, (const uint8_t*)buf);
	}
	
		
	size_t DeviceFileAdapter::capacity() const
	{
		return file->size();
	}
	
}