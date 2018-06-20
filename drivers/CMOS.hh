#ifndef INCLUDED_CMOS_HH
#define INCLUDED_CMOS_HH

#include <Common.h>

extern "C" {
	struct tm;
}

namespace Drivers
{
	
	// Static/singleton class
	class alignas(bool) CMOS final
	{
		private:
		static CMOS instance;
		bool initialized : 1;
		bool NMI_disabled : 1;
		bool f24 : 1;
		bool fBinary : 1;
		
		
		CMOS() noexcept;
		
		void init() noexcept;
		unsigned char read(unsigned char reg) const noexcept;
		void write(unsigned char reg, unsigned char data) const noexcept;
		
		struct tm readTime() noexcept;
		
		constexpr static unsigned char BCD_to_binary(const unsigned char bcd)
		{
			return (bcd & 0xF0 >> 1) + ((bcd & 0xF0) >> 3) + (bcd & 0x0F);
		}
		
		
		public:
		constexpr static unsigned char SelectPort = 0x70;
		constexpr static unsigned char IOPort = 0x71;
		
		constexpr static unsigned char NMI_bit = 0x80;
		
		constexpr static unsigned char r_Seconds = 0x00;
		constexpr static unsigned char r_Minutes = 0x02;
		constexpr static unsigned char r_Hours = 0x04;
		constexpr static unsigned char r_Weekday = 0x06;
		constexpr static unsigned char r_DayOfMonth = 0x07;
		constexpr static unsigned char r_Month = 0x08;
		constexpr static unsigned char r_Year = 0x09;
		constexpr static unsigned char r_Century = 0x32;
		constexpr static unsigned char r_StatusA = 0x0A;
		constexpr static unsigned char r_StatusB = 0x0B;
		constexpr static unsigned char r_StatusC = 0x0C;
		
		constexpr static unsigned char Format_24h = 0x02;
		constexpr static unsigned char Format_Binary = 0x04;
		
		constexpr static unsigned char RTC_i = 0x08;
		constexpr static unsigned char RTC_IRQ_Bit = 0x40;
		
		// In Hz
		constexpr static uint16_t RTC_Default_Freq = 1024;
		
		
		static bool DisableNMI() noexcept;
		static bool EnableNMI() noexcept;
		
		__attribute__((always_inline))
		static bool NMIDisabled() noexcept
		{ return instance.NMI_disabled; }
		
		
		static struct tm GetTime(time_t* = nullptr) noexcept;
		
		static bool SetRTCInterrupts(bool enabled) noexcept;
	};
	
	static_assert(sizeof(CMOS) == 1);
	
	
}

#endif