#ifndef INCLUDED_BITS_DURATION_HH
#define INCLUDED_BITS_DURATION_HH

#include <Common.h>
#include <Utils/ratio>
#include "common_type.hh"
#include "../numeric_limits.hh"

namespace Utils { namespace chrono
{
	template <class Rep>
	struct duration_values
	{
		static constexpr Rep zero()
		{
			return Rep(0);
		}
		
		static constexpr Rep min()
		{
			return numeric_limits<Rep>::lowest();
		}
		
		static constexpr Rep max()
		{
			return numeric_limits<Rep>::max();
		}
	};
	
	
	
	
	
	
	template <class Rep, class Period = ratio<1>>
	class duration
	{
		public:
		typedef Rep rep;
		typedef Period period;
		
		protected:
		rep ticks;
		
		public:
		
		constexpr duration() = default;
		duration(const duration&) = default;
		template <class Rep2>
		constexpr explicit duration(const Rep2& r);
		template <class Rep2, class Period2>
		constexpr duration(const duration<Rep2, Period2>&);
		
		
		duration& operator=(const duration&) = default;
		
		
		constexpr rep count() const
		{
			return ticks;
		}
		
		static constexpr duration zero()
		{
			return duration(duration_values<rep>::zero());
		}
		
		static constexpr duration min()
		{
			return duration(duration_values<rep>::min());
		}
		
		static constexpr duration max()
		{
			return duration(duration_values<rep>::max());
		}
		
		constexpr common_type_t<duration> operator+() const;
		constexpr common_type_t<duration> operator-() const;
		constexpr duration& operator++();
		constexpr duration operator++(int);
		constexpr duration& operator--();
		constexpr duration operator--(int);
		constexpr duration& operator+=(const duration&);
		constexpr duration& operator-=(const duration&);
		constexpr duration& operator*=(const rep&);
		constexpr duration& operator/=(const rep&);
		constexpr duration& operator%=(const rep&);
		constexpr duration& operator%=(const duration&);
		
		
	};
	
	template <class R1, class P1, class R2, class P2>
	constexpr bool operator==(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(d1).count() == CT(d2).count();
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr bool operator!=(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(d1).count() != CT(d2).count();
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr bool operator<=(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(d1).count() <= CT(d2).count();
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr bool operator<(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(d1).count() < CT(d2).count();
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr bool operator>=(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(d1).count() >= CT(d2).count();
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr bool operator>(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(d1).count() > CT(d2).count();
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr common_type_t<duration<R1, P1>, duration<R2, P2>> operator+(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(CT(d1).count() + CT(d2).count());
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr common_type_t<duration<R1, P1>, duration<R2, P2>> operator-(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(CT(d1).count() - CT(d2).count());
	}
	
	
	
	template <class R1, class P1, class R2>
	constexpr duration<common_type_t<R1, R2>, P1> operator*(const duration<R1, P1>& d1, const R2& d2)
	{
		typedef duration<common_type_t<R1, R2>, P1> CT;
		return CT(CT(d1).count() * d2);
	}
	
	
	
	template <class R1, class P1, class R2>
	constexpr duration<common_type_t<R1, R2>, P1> operator*(R2& d2, const duration<R1, P1>& d1)
	{
		typedef duration<common_type_t<R1, R2>, P1> CT;
		return CT(CT(d1).count() * d2);
	}
	
	
	
	template <class R1, class P1, class R2>
	constexpr duration<common_type_t<R1, R2>, P1> operator/(const duration<R1, P1>& d1, const R2& d2)
	{
		typedef duration<common_type_t<R1, R2>, P1> CT;
		return CT(CT(d1).count() / d2);
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr common_type_t<duration<R1, P1>, duration<R2, P2>> operator/(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(CT(d1).count() / CT(d2).count());
	}
	
	
	
	template <class R1, class P1, class R2>
	constexpr duration<common_type_t<R1, R2>, P1> operator%(const duration<R1, P1>& d1, const R2& d2)
	{
		typedef duration<common_type_t<R1, R2>, P1> CT;
		return CT(CT(d1).count() % d2);
	}
	
	
	
	template <class R1, class P1, class R2, class P2>
	constexpr common_type_t<duration<R1, P1>, duration<R2, P2>> operator%(const duration<R1, P1>& d1, const duration<R2, P2>& d2)
	{
		typedef common_type_t<duration<R1, P1>, duration<R2, P2>> CT;
		return CT(CT(d1).count() % CT(d2).count());
	}
	
	
	
	
	
	
	
	
}
	
	
	
	
	
	
	
	template <class R1, class P1, class R2, class P2>
	struct common_type<chrono::duration<R1, P1>, chrono::duration<R2, P2>>
	{
		private:
		static constexpr intmax_t gcd_num = detail::static_gcd<P1::num, P2::num>::value;
		static constexpr intmax_t gcd_den = detail::static_gcd<P1::den, P2::den>::value;
		public:
		
		typedef chrono::duration<common_type_t<R1, R2>, ratio<gcd_num, (P1::den / gcd_den) * P2::den>> type;
	};
}


#endif