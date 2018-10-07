#ifndef INCLUDED_FS_DEVICETARGET_DRIVER_HH
#define INCLUDED_FS_DEVICETARGET_DRIVER_HH

#include "DeviceTarget.hh"
#include <Utils/shared_ptr>

namespace Drivers
{
	class Driver;
	class Device;
}


namespace Kernel::FS
{
	class DriverTarget : public DeviceTarget
	{
		public:
		
		protected:
		const Drivers::Driver* driver;
		Drivers::Device* target;
		const uint32_t minor;
		
		DriverTarget(const Drivers::Driver*, Drivers::Device*, const uint32_t minor) noexcept;
		
		public:
		
		virtual bool identifiers(bool* block, uint32_t* driver, uint32_t* device) const noexcept override;
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		
		static Utils::shared_ptr<DriverTarget> Make(const Drivers::Driver*, const uint32_t minor);
	};
	
}

#endif