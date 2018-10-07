#ifndef INCLUDED_DRIVERS_DEVICE_HH
#define INCLUDED_DRIVERS_DEVICE_HH

namespace Drivers
{
	class Device
	{
		public:
		
		protected:
		
		public:
		
		virtual size_t read(size_t start, size_t len, void* buf) const = 0;
		virtual size_t write(size_t start, size_t len, const void* buf) = 0;
		virtual bool is_char() const noexcept = 0;
	};
	
}


#endif