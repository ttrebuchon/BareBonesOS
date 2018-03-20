#ifndef INCLUDED_BOUNDS_HH
#define INCLUDED_BOUNDS_HH

#include <Common.h>
#include <Utils/iterator>

namespace Utils
{
	template <class It, class T>
	It lower_bound(It start, It end, const T& value)
	{
		typedef iterator_traits<It> Traits;
		typename Traits::difference_type count, step;
		It it;
		
		count = distance(start, end);
		while (count > 0)
		{
			it = start;
			step = count / 2;
			advance(it, step);
			if (*it < value)
			{
				start = ++it;
				count -= step + 1;
				
			}
			else
			{
				count = step;
			}
			
		}
		return start;
	}
	
}

#endif