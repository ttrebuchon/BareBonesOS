#ifndef INCLUDED_CHRONO_HH
#define INCLUDED_CHRONO_HH

#include <Common.h>
#include "bits/duration.hh"
#include "bits/duration.hpp"

namespace Utils { namespace chrono
{
	
	
	
	typedef duration<int64_t> seconds;
	
	
	
	
	template <class Clock, class Duration = typename Clock::duration>
	class time_point
	{
		public:
		typedef Clock clock;
		typedef Duration duration;
		typedef typename duration::rep rep;
		typedef typename duration::period period;
		
		protected:
		duration counter;
		
		public:
		constexpr time_point();
		constexpr explicit time_point(const duration& d);
		template <class D2>
		constexpr time_point(const time_point<clock, D2>& t) : counter(D2::period*t.ticks/duration::period)
		{
			// TODO /  uncertain
		}
		
		constexpr duration time_since_epoch() const;
	};
	
	
	
	
	
	class system_clock
	{
		public:
		typedef int64_t rep;
		typedef ratio<1> period;
		typedef chrono::duration<rep, period> duration;
		typedef chrono::time_point<system_clock> time_point;
		
		static constexpr bool is_steady = false;
		
		static_assert(!is_steady);
		static_assert(duration::zero().count() == 0);
		static_assert(duration::min() < duration::zero());
		
		protected:
		
		
		public:
		static time_point now() noexcept;
		
		
		static time_point from_time_t(time_t t) noexcept
		{
			return time_point(seconds(t));
		}
		static time_t to_time_t(time_point) noexcept
		{
			// TODO
			assert(false);
		}
		
	};
	
	
	
	static system_clock::time_point from_time_t(time_t) noexcept;
	
	
	
	
	class steady_clock
	{
		public:
		typedef int64_t rep;
		typedef ratio<1> period;
		typedef chrono::duration<rep, period> duration;
		typedef chrono::time_point<steady_clock> time_point;
		
		static constexpr bool is_steady = true;
		static_assert(duration::zero().count() == 0);
		static_assert(duration::min() < duration::zero());
		
		protected:
		
		
		public:
		static time_point now() noexcept;
		
		
		static time_point from_time_t(time_t t) noexcept
		{
			return time_point(seconds(t));
		}
		static time_t to_time_t(time_point) noexcept
		{
			// TODO
			assert(false);
		}
		
	};
}
}

#endif