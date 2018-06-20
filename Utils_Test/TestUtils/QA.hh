#pragma once
#include <iostream>
#include <Common.h>

namespace Drivers
{
	class Disk;
}

class QA
{
	private:
	static void Out_Init();
	static void MultiBoot_Init();
	static void Timer_Init();
	static void Filesystem_Init();
	static void Paging_Init();
	
	static void* phys_start;
	static void* phys_end;
	
	static timer_t PIC_timer;
	static struct itimerspec PIC_config;
	
	protected:
	
	class _ostream;
	
	
	
	public:
	const static int PIC_signal;
	
	static std::ostream out;
	
	class Memory;
	
	static void Init();
	
	static const void* physical_start() noexcept
	{
		return phys_start;
	}
	
	static const void* physical_end() noexcept
	{
		return phys_end;
	}
	
	static void DisableTimer();
	static void EnableTimer();
	static void SetTimerInterval_ns(unsigned long long nanoseconds);
	
	static void SetTimerInterval_ms(unsigned long long milliseconds)
	{
		SetTimerInterval_ns(milliseconds*1000*1000);
	}
	
	static void CheckPIC();
	
	static timer_t GetPICTimer()
	{ return PIC_timer; }
	
	static Drivers::Disk* QADrive(const char* file, const size_t size);
};