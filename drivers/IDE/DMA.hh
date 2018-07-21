#ifndef INCLUDED_DMA_HH
#define INCLUDED_DMA_HH

#include <Common.h>
#include "PRDT.hh"
#include "IDEDisk.hh"
#include "IDE.hh"
#include <kernel/Registers.h>

namespace Drivers { namespace IDE {
	
	
	class DMADrive : public Disk
	{
		public:
		enum class Direction : bool
		{
			Write = false,
			Read = true
		};
		
		struct alignas(unsigned char) Status
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

			static Status Get(unsigned char byte)
			{
				return *reinterpret_cast<Status*>(&byte);
			}
		};
		
		static_assert(sizeof(Status) == 1);
		
		constexpr static size_t PRDT_Size = 10;
		
		private:
		static bool _initted;
		static void Initialize();

		static void HandleInterrupt(Registers_t);
		
		// Second bus is offset by 0x8
		enum class Register : uint16_t
		{
			Command = 0x0,
			Status = 0x2,
			PRDT_Addr = 0x4,
		};
		
		enum class Function : uint16_t
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
		mutable PRDT<PRDT_Size>* prdt;
		addr_t prdt_phys;
		mutable unsigned char* bufs[PRDT_Size];
		mutable addr_t bufsPhys[PRDT_Size];
		mutable size_t bufsSize[PRDT_Size];
		
		
		void configureForSize(size_t len) const;
		
		public:
		
		
		
		void toggle(bool nState);
		void direction(Direction);
		
		DMADrive(Device*);
		DMADrive(const Channel, const Role);
		
		unsigned char* readSector(uint32_t lba, uint32_t len);
		
		virtual int read(size_t start, size_t len, unsigned char* buf) const;
		virtual int write(size_t start, size_t len, const unsigned char* buf);
		
		virtual size_t capacity() const noexcept;
	};
	
}
}

#endif