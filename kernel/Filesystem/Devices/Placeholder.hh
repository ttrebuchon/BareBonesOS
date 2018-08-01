#ifndef INCLUDED_FS_DEVICES_PLACEHOLDER_HH
#define INCLUDED_FS_DEVICES_PLACEHOLDER_HH

#include "DeviceTarget.hh"



namespace Kernel::FS
{
	
	class PlaceholderDevice : public DeviceTarget
	{
		
		private:
		uint32_t major, minor;
		bool block;
		
		public:
		PlaceholderDevice(bool block, uint32_t, uint32_t);
		
		virtual bool identifiers(bool* block, uint32_t* driver, uint32_t* device) const noexcept override;
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
	};
	
}


#endif