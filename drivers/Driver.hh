#ifndef INCLUDED_DRIVERS_DRIVER_HH
#define INCLUDED_DRIVERS_DRIVER_HH

#include "Driver.h"

#include <Support/MetaProgramming/Utilities.hh>
#include <Support/threading/LowMutex.hh>

namespace Drivers
{
	typedef uint32_t driver_t;
	typedef int64_t driverf_t;
	
	
	#define BLOCK_DRIVER_ID_OFF 65535
	
	
	
	class Device;
	
	class Driver
	{
		public:
		
		protected:
		
		public:
		
		virtual driver_t devices() const noexcept = 0;
		virtual Device* device(const driver_t) const noexcept = 0;
		
		virtual driver_t major() const noexcept = 0;
	};
	
	
	class DriverManager final
	{
		public:
		constexpr static driver_t ndriver = 0;
		typedef Support::threading::low_mutex mutex_type;
		
		private:
		class DriverSet;
		static DriverSet* drivers;
		static mutex_type drivers_m;
		
		static bool init_drivers() noexcept;
		
		public:
		
		static Driver* at(const driverf_t index) noexcept;
		static Driver* at(const driver_t index, bool block) noexcept;
		static Driver* block_at(const driver_t index) noexcept
		{ return at(index, true); }
		static Driver* char_at(const driver_t index) noexcept
		{ return at(index, false); }
		
		static driverf_t register_man_driver(const driverf_t, Driver*) noexcept;
		static driver_t register_char_driver(const driver_t, Driver*) noexcept;
		static driver_t register_block_driver(const driver_t, Driver*) noexcept;
	};
	


}
#endif