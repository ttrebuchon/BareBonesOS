#ifndef INCLUDED_SUPPORT_METAPROGRAMMING_NUMBERS_HH
#define INCLUDED_SUPPORT_METAPROGRAMMING_NUMBERS_HH

#include <Common.h>
#include <Utils/utility>
#include <Utils/typeindex>

namespace Support { namespace Meta
{
	template <class T, T...>
	struct number_sequence;
	
	
	namespace detail
	{
		template <class T, T, bool B, T... values>
		struct __number_sequence_from_number;
		
		template <class T, T last, T... values>
		struct __number_sequence_from_number<T, last, true, values...>
		{
			typedef number_sequence<T, last, values...> type;
		};
		
		template <class T, T max, T... values>
		struct __number_sequence_from_number<T, max, false, values...>
		{
			typedef typename __number_sequence_from_number<T, max-1, ((max-1) == 0), max, values...>::type type;
		};
	}
	
	
	template <class T, T number, bool B = (number == 0)>
	struct number_sequence_from_number
	{
		static_assert(number > 0);
		typedef typename detail::__number_sequence_from_number<T, number-1, ((number-1)==0)>::type type;
	};
	
	template <class T, T n>
	struct number_sequence_from_number<T, n, true>
	{
		typedef number_sequence<T> type;
	};
	
}
}


#endif
	