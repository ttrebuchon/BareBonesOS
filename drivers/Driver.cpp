#include "Driver.hh"
#include <Std/errno.h>
#include <Utils/mutex>
#include <kernel/Memory/Allocators/cross_processor_allocator.hh>
#include <Utils/map>

namespace Drivers
{
	typedef DriverManager DriverMngr;
	
	
	class DriverMngr::DriverSet
	{
		public:
		//typedef Support::threading::low_mutex mutex_type;
		typedef Utils::mutex mutex_type;
		
		private:
		Utils::map<driverf_t, Driver*, Utils::less<driverf_t>, Kernel::Memory::cross_proc_allocator<Utils::pair<driverf_t, Driver*>>> drivers;
		mutex_type drivers_m;
		
		~DriverSet() = delete;
		
		public:
		DriverSet() noexcept
		{
			
		}
		
		driver_t add(const driverf_t major, Driver* driver) noexcept
		{
			Utils::lock_guard<mutex_type> lock(drivers_m);
			if (drivers.count(major))
			{
				auto other = drivers.at(major);
				if (other == driver)
				{
					return major;
				}
				else
				{
					tset_error(EINVAL, "A driver is already registered with the provided 'major' identifier.");
					return ndriver;
				}
			}
			else
			{
				drivers[major] = driver;
				return major;
			}
		}
		
		Driver* get(const driverf_t id) noexcept
		{
			Utils::lock_guard<mutex_type> lock(drivers_m);
			if (drivers.count(id))
			{
				return drivers.at(id);
			}
			else
			{
				return nullptr;
			}
			assert(NOT_IMPLEMENTED);
		}
	};
	
	
	
	
	
	typename DriverMngr::DriverSet* DriverMngr::drivers = nullptr;
	typename DriverMngr::mutex_type DriverMngr::drivers_m;
	
	#define CHECK_INIT(...)  \
	if (!drivers) \
	{ \
		if (!init_drivers()) \
		{ \
			return __VA_ARGS__; \
		} \
	}
	
	
	Driver* DriverMngr::at(const driverf_t id) noexcept
	{
		/*if (!drivers)
		{
			Utils::lock_guard<mutex_type> lock(drivers_m);
			if (!drivers)
			{
				if (!init_drivers())
				{
					return nullptr;
				}
			}
		}*/
		CHECK_INIT(nullptr);
		
		auto driver = drivers->get(id);
		return driver;
		
		assert(NOT_IMPLEMENTED);
	}
	
	Driver* DriverMngr::at(const driver_t id, bool block) noexcept
	{
		//return at((driverf_t)id + (block ? BLOCK_DRIVER_ID_OFF : 0));
		return at(((driverf_t)id)*(block ? -1 : 1));
	}
	
	
	driverf_t DriverMngr::register_man_driver(const driverf_t major, Driver* driver) noexcept
	{
		CHECK_INIT(ndriver);
		
		if (!driver)
		{
			tset_error(EINVAL, "Cannot register a null driver.");
			return ndriver;
		}
		else
		{
			return drivers->add(major, driver);
		}
	}
	
	driver_t DriverMngr::register_char_driver(const driver_t major, Driver* driver) noexcept
	{
		return (driver_t)register_man_driver(major, driver);
	}
	
	driver_t DriverMngr::register_block_driver(const driver_t major, Driver* driver) noexcept
	{
		return (driver_t)(-1*register_man_driver(-1*(driverf_t)major, driver));
	}
	
	
	
	
	bool DriverMngr::init_drivers() noexcept
	{
		Utils::lock_guard<mutex_type> lock(drivers_m);
		if (drivers)
		{
			return true;
		}
		
		Kernel::Memory::cross_proc_allocator<DriverSet> alloc;
		
		drivers = alloc.allocate(1);
		if (unlikely(!drivers))
		{
			return false;
		}
		alloc.construct(drivers);
		
		return true;
	}
}