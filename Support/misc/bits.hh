#ifndef INCLUDED_SUPPORT_MISC_BITS_HH
#define INCLUDED_SUPPORT_MISC_BITS_HH

#include <Common.h>


namespace Support::misc
{
	
	template <class T>
	size_t highest_bit_index(const T n)
	{
		assert(n > 0);
		size_t clz = 0;
		if (sizeof(T) <= sizeof(unsigned int))
		{
			return sizeof(unsigned int)*8 - 1 - __builtin_clz(n);
		}
		else if (sizeof(T) <= sizeof(unsigned long))
		{
			return sizeof(unsigned long)*8 - 1 - __builtin_clzl(n);
		}
		else if (sizeof(T) <= sizeof(unsigned long long))
		{
			clz = __builtin_clzll(n);
			return sizeof(unsigned long long)*8 - 1 - __builtin_clzll(n);
		}
		
		static_assert(sizeof(T) <= sizeof(unsigned long long));
		
		
	}
	
	
}


#endif