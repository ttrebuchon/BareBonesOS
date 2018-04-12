#include "CMOS.hh"
#include "PortIO.h"
#include <Std/ctime>
#include <kernel/Interrupts.h>

namespace Drivers
{
	CMOS CMOS::instance;
	
	CMOS::CMOS() : initialized(false), NMI_disabled(false), f24(false), fBinary(false)
	{
		
	}
	
	void CMOS::init() noexcept
	{
		initialized = true;
		const unsigned char B = read(r_StatusB);
		
		if (B & Format_24h)
		{
			f24 = true;
		}
		
		if (B & Format_Binary)
		{
			fBinary = true;
		}
	}
	
	unsigned char CMOS::read(unsigned char reg) const noexcept
	{
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		
		
		return port_byte_in(IOPort);
	}
	
	void CMOS::write(unsigned char reg, unsigned char data) const noexcept
	{
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		port_byte_out(SelectPort, (((unsigned char)NMI_disabled) << 7) | reg);
		
		
		port_byte_out(IOPort, data);
	}
	
	tm CMOS::readTime() noexcept
	{
		if (!initialized)
		{
			init();
		}
		
		bool mismatch = false;
		unsigned char values[12];
		do
		{
			values[0] = read(r_Seconds);
			values[1] = read(r_Minutes);
			values[2] = read(r_Hours);
			values[3] = read(r_DayOfMonth);
			values[4] = read(r_Month);
			values[5] = read(r_Year);
			
			values[6] = read(r_Seconds);
			values[7] = read(r_Minutes);
			values[8] = read(r_Hours);
			values[9] = read(r_DayOfMonth);
			values[10] = read(r_Month);
			values[11] = read(r_Year);
			
			mismatch = false;
			for (uint8_t i = 0; i < 6 && !mismatch; ++i)
			{
				mismatch |= (values[i] != values[i+6]);
			}
		}
		while (mismatch);
		
		bool pm = false;
		
		if (!f24)
		{
			pm = (values[2] & 0x80);
		}
		
		if (!fBinary)
		{
			values[0] = BCD_to_binary(values[0]);
			values[1] = BCD_to_binary(values[1]);
			values[2] = BCD_to_binary(values[2]);
		}
		
		if (pm)
		{
			values[2] = (values[2] + 12) % 24;
		}
		
		uint16_t year = values[5] + 2000;
		
		if (year < __YEAR__)
		{
			year += 100;
		}
		
		tm t;
		t.tm_sec = values[0];
		t.tm_min = values[1];
		t.tm_hour = values[2];
		t.tm_mday = values[3];
		t.tm_mon = values[4];
		t.tm_year = year - 1900;
		//t.tm_wday = ...
		//t.tm_yday = ...
		t.tm_isdst = -1;
		
		return t;
	}
	
	
	
	
	
	
	
	
	
	bool CMOS::DisableNMI() noexcept
	{
		Kernel::irq_guard lock;
		instance.NMI_disabled = true;
		instance.read(r_StatusB);
		return true;
	}
	
	bool CMOS::EnableNMI() noexcept
	{
		Kernel::irq_guard lock;
		instance.NMI_disabled = false;
		instance.read(r_StatusB);
		return true;
	}
	
	
	tm CMOS::GetTime(time_t* t) noexcept
	{
		tm obj = instance.readTime();
		auto res = mktime(&obj);
		ASSERT(res != -1);
		if (t)
		{
			*t = res;
		}
		return obj;
	}
	
	bool CMOS::SetRTCInterrupts(bool enable) noexcept
	{
		bool prevNMI = NMIDisabled();
		Kernel::irq_guard lock;
		if (!prevNMI)
		{
			DisableNMI();
		}
		
		auto prevVal = instance.read(r_StatusB);
		bool prevStat = (prevVal & RTC_IRQ_Bit);
		if (prevStat && !enable)
		{
			prevVal = prevVal & ~(RTC_IRQ_Bit);
			instance.write(r_StatusB, prevVal);
		}
		else if (!prevStat && enable)
		{
			prevVal = prevVal | RTC_IRQ_Bit;
			instance.write(r_StatusB, prevVal);
		}
		
		if (!prevNMI)
		{
			EnableNMI();
		}
		return prevStat;
	}
}