#ifndef INCLUDED_PHYSICAL_MEMORY_HH
#define INCLUDED_PHYSICAL_MEMORY_HH

#include <Common.h>
//#include <Utils/vector>
#include <Utils/Bitset.hh>

namespace boot
{
	class MultiBoot;
	
	extern MultiBoot* mboot;
}

namespace Kernel { namespace Memory
{
	class PMM;
	
	class PhysicalMemory final
	{
		public:
		//typedef Utils::vector<bool> frames_type;
		typedef Utils::Bitset_Ptr<> frames_type;
		
		private:
		static bool changing;
		static PMM* under;
		static frames_type frames;
		
		static void Initialize() noexcept;
		
		static bool NotifyUsed(const addr_t) noexcept;
		
		constexpr static size_t Index(const addr_t& a) noexcept
		{
			return a/PageSize;
		}
		
		constexpr static size_t PageCount(const addr_t& a, const addr_t& len) noexcept
		{
			return Index(a+len) - Index(a) + (a + len % PageSize != 0 ? 1 : 0);
		}
		
		public:
		constexpr static size_t PageSize = 0x1000;
		
		template <class T>
		static bool Use() noexcept
		{
			irq_guard lock;
			assert(!changing);
			changing = true;
			if (under)
			{
				delete under;
			}
			under = new T(frames);
			changing = false;
			return true;
		}
		
		template <class Fn>
		static bool Use(Fn fn) noexcept
		{
			irq_guard lock;
			assert(!changing);
			changing = true;
			auto nUnder = fn(frames);
			if (!nUnder)
			{
				changing = false;
				return false;
			}
			
			if (under)
			{
				delete under;
			}
			under = nUnder;
			changing = false;
			return true;
		}
		
		// Modifies "size" to equal the
		// the actual size reserved
		static addr_t reserveFirstFree(size_t& size) noexcept;
		
		// Modifies "size" to equal the
		// the actual size reserved
		static addr_t reserve(size_t& size) noexcept;
		
		static bool reserve(addr_t start, addr_t len) noexcept;
		
		static bool release(addr_t addr, size_t sz) noexcept;
		
		
		friend class PMM;
	};
	
	
}
}

#endif