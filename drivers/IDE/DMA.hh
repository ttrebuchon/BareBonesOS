#ifndef INCLUDED_DMA_HH
#define INCLUDED_DMA_HH

#include <Common.h>
#include "PRDT.hh"
#include "IDEDisk.hh"
#include "IDE.hh"

namespace Drivers { namespace IDE {
	
	
	class DMADrive : public Disk
	{
		public:
		enum class Direction : bool
		{
			Write = false,
			Read = true
		};
		
		struct alignas(char) Status
		{
			bool obsolete : 1;
			char biosDMA : 2;
			char unknown : 2;
			bool irqGenerated : 1;
			bool failed : 1;
			bool done : 1;
			
			operator unsigned char() const
			{
				return *reinterpret_cast<const unsigned char*>(this);
			}
		};
		
		static_assert(sizeof(Status) == 1);
		
		private:
		
		// Second bus is offset by 0x8
		enum class Register
		{
			Command = 0x0,
			Status = 0x2,
			PRDT_Addr = 0x4,
		};
		
		enum class Function
		{
			Read48 = 0x25,
			Write48 = 0x35,
			
			
			Read28 = 0xC8,
			Write28 = 0xCA,
		};
		
		
		uint32_t BMR;
		Direction _dir;
		bool _state;
		Device* dev;
		PRDT<10>* prdt;
		addr_t prdt_phys;
		
		public:
		
		
		
		void toggle(bool nState);
		void direction(Direction);
		
		DMADrive(Device*);
		DMADrive(const Channel, const Role);
	};
	
}
}

#endif