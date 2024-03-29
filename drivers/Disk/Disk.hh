#ifndef INCLUDED_DISK_HH
#define INCLUDED_DISK_HH

#include <Common.h>

namespace Drivers {
	
	class Disk
	{
		private:
		
		
		public:
		
		virtual int read(size_t start, size_t len, unsigned char* buf) const = 0;
		virtual int write(size_t start, size_t len, const unsigned char* buf) = 0;
		virtual size_t write(size_t start, uint8_t value, size_t len);
		
		virtual size_t capacity() const noexcept = 0;
	};
	
}
#endif