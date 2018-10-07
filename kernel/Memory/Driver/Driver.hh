#ifndef INCLUDED_KERNEL_MEMORY_DRIVER_DRIVER_HH
#define INCLUDED_KERNEL_MEMORY_DRIVER_DRIVER_HH

#include <drivers/Driver.hh>


#define CHAR_MEM 0x1

#define CHAR_MEM_MEM 0x1 // Physical memory
#define CHAR_MEM_KMEM 0x2 // Kernel memory
#define CHAR_MEM_NULL 0x3
#define CHAR_MEM_PORT 0x4 // IO Port
#define CHAR_MEM_ZERO 0x5
#define CHAR_MEM_CORE 0x6 // Obsolete
// Always return ENOSPC on write
#define CHAR_MEM_FULL 0x7
#define CHAR_MEM_RANDOM 0x8
#define CHAR_MEM_URANDOM 0x9
#define CHAR_MEM_AIO 0xA
#define CHAR_MEM_KMSG 0xB
#define CHAR_MEM_OLD_MEM 0xC // Obsolete

namespace Kernel::Memory
{
	class MemoryDriver final : public Drivers::Driver
	{
		private:
		
		static MemoryDriver _instance;
		static Drivers::Device* dev_instances[];
		
		MemoryDriver() noexcept;
		
		public:
		constexpr static Drivers::driver_t Major = 0x1;
		
		virtual Drivers::driver_t devices() const noexcept override;
		virtual Drivers::Device* device(const Drivers::driver_t) const noexcept override;
		
		virtual Drivers::driver_t major() const noexcept override
		{
			return Major;
		}
		
		constexpr static MemoryDriver* instance() noexcept
		{
			return &_instance;
		}
	};
}



#endif