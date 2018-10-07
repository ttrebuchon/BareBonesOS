#ifndef INCLUDED_FS_DEVICETARGET_HH
#define INCLUDED_FS_DEVICETARGET_HH

#include <Common.h>
#include "Signature.h"
#include <Utils/shared_ptr>


namespace Kernel::FS
{
	
	enum class DeviceTargetType
	{
		Char = 0,
		Block = 1,
	};
	
	class Filesystem;
	
	class DeviceTarget
	{
		public:
		
		protected:
		
		public:
		
		virtual bool identifiers(bool* block, uint32_t* driver, uint32_t* device) const noexcept = 0;
		bool identifiers(bool* block, dev_t*) const noexcept;
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) = 0;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) = 0;
		
		
		static Utils::shared_ptr<DeviceTarget> find_target(Filesystem*, const DeviceTargetType, const uint32_t major, const uint32_t minor) noexcept;
		static Utils::shared_ptr<DeviceTarget> find_target(Filesystem*, const DeviceTargetType, const dev_t sig) noexcept;
	};
	
	typedef Utils::shared_ptr<DeviceTarget> devtarget_t;
	
}

#endif
	