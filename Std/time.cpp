#include "ctime"
#include <Utils/limits>
#include <drivers/CMOS.hh>

#undef __localtime_r
#define __localtime_r localtime_r

extern "C" {
	
	#define SHR(a, b) \
	((-1 >> 1 == -1 \
	&& \
	(long int)-1 >> 1 == -1 \
	&& \
	((time_t)-1 >> 1 == -1 || !(Utils::numeric_limits<time_t>::is_signed)) \
	) \
	? (a) >> (b) \
	: (a) / (1 << (b)) - ((a) % (1 << (b)) < 0))
	constexpr unsigned short int __mon_yday[2][13] = {
		// Normal
		{
			0, 31, 59, 90, 120, 151, 181,
			212, 243, 273, 304, 334, 365
		},
		
		// Leap
		{
			0, 31, 60, 91, 121, 152, 182,
			213, 244, 274, 305, 335, 366
		}
	};
	
	static inline bool leapyear(long year)
	{
		return (((year & 3) == 0) && (year % 100 != 0 || ((year / 100) & 3) == (- (TM_YEAR_BASE / 100) & 3)));
	}
	
	
	static inline time_t ydhms_diff(long y1, long yday1, long h1, long m1, long s1, int y0, int yday0, int h0, int m0, int s0)
	{
		assert(-1/2 == 0);
		
		int a4 = SHR(y1, 2) + SHR(TM_YEAR_BASE, 2) - !(y1 & 3);
		int b4 = SHR(y0, 2) + SHR(TM_YEAR_BASE, 2) - !(y0 & 3);
		int a100 = a4/25 - (a4 % 25 < 0);
		int b100 = b4/25 - (b4 % 25 < 0);
		int a400 = SHR(a100, 2);
		int b400 = SHR(b100, 2);
		
		int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);
		
		time_t ty1 = y1;
		time_t years = ty1 - y0;
		time_t days = 365*years + yday1 - yday0 + intervening_leap_days;
		time_t hours = 24*days + h1 - h0;
		time_t minutes = 60*hours + m1 - m0;
		time_t seconds = 60*minutes + s1 - s0;
		return seconds;
	}
	
	typedef int locoff_t;
	static locoff_t localtime_offset;
	
	time_t __mktime(tm* tp, tm*(*convert)(const time_t*, tm*), locoff_t* offset)
	{
		time_t t, gt, t0, t1, t2;
		struct tm tm;
		int remaining_probes = 6;
		int sec = tp->tm_sec;
		int min = tp->tm_min;
		int hour = tp->tm_hour;
		int mday = tp->tm_mday;
		int mon = tp->tm_mon;
		int year_req = tp->tm_year;
		int isdst = tp->tm_isdst;
		
		int dst2;
		int mon_rem = mon % 12;
		int neg_mon_rem = mon_rem < 0;
		int mon_years = mon/12 - neg_mon_rem;
		
		long lyear_req = year_req;
		long year = lyear_req + mon_years;
		
		int mon_yday = ((__mon_yday[leapyear(year)][mon_rem + 12*neg_mon_rem]) - 1);
		
		long lmday = mday;
		long yday = mon_yday + lmday;
		
		time_t guessed_off = *offset;
		
		int sec_req = sec;
		
		if (sec < 0)
		{
			sec = 0;
		}
		if (59 < sec)
		{
			sec = 59;
		}
		
		t0 = ydhms_diff(year, yday, hour, min, sec, EPOCH_YEAR - TM_YEAR_BASE, 0, 0, 0, -guessed_off);
		
		tp->tm_sec = sec;
		tp->tm_min = min;
		tp->tm_hour = hour;
		tp->tm_mon = mon_rem;
		tp->tm_year = year;
		tp->tm_yday = yday;
		
		// TODO
		
		return t0;
	}
	
	time_t mktime(tm* t)
	{
		return __mktime(t, /*__localtime_r*/nullptr, &localtime_offset);
	}
	
	
	time_t time(time_t* t)
	{
		time_t rt;
		Drivers::CMOS::GetTime(&rt);
		if (t)
		{
			*t = rt;
		}
		return rt;
	}
	
}