#ifndef INCLUDED_UTILS_ARRAY_HH
#define INCLUDED_UTILS_ARRAY_HH

#include <Common.h>
#include "functexcept.hh"

namespace Utils
{
	template <class T, size_t N>
	struct array
	{
		public:
		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef pointer iterator;
		typedef const_pointer const_iterator;
		
		
		protected:
		T _array[N];
		
		
		public:
		
		
		constexpr reference at(size_type pos)
		{
			if (pos >= N)
			{
				__throw_out_of_range("Index out of range");
			}
			
			return _array[pos];
		}
	};
}



#endif