#ifndef INCLUDED_ITERATOR_HPP
#define INCLUDED_ITERATOR_HPP

#include "Iterator.hh"
#include "streambuf.hh"

namespace Utils {
	
	template <class T, class Traits>
	void ostreambuf_iterator<T, Traits>::_put(const T* str, size_t len)
	{
		if (!failbit)
		{
			if (buf->sputn(str, len) != len)
			{
				failbit = true;
			}
		}
		
	}
	
	
	
	
	
	
	
	namespace detail
	{
		template <class Iter, class = void>
		struct distance_helper;
		
		template <class Iter>
		struct distance_helper<Iter, random_access_iterator_tag>
		{
			
			static inline constexpr typename iterator_traits<Iter>::difference_type call(Iter first, Iter last)
			{
				return last - first;
			}
		};
		
		template <class Iter, class>
		struct distance_helper
		{
			
			static inline constexpr typename iterator_traits<Iter>::difference_type call(Iter first, Iter last)
			{
				typename iterator_traits<Iter>::difference_type n = 0;
				while (first != last)
				{
					++n;
					++first;
				}
				return n;
			}
		};
		
		
		template <class Iter, class Distance, class = void>
		struct advance_helper;
		
		template <class Iter, class Distance>
		struct advance_helper<Iter, Distance, random_access_iterator_tag>
		{
			static inline constexpr void call(Iter& it, Distance n)
			{
				it = it + n;
			}
		};
		
		template <class Iter, class Distance, class>
		struct advance_helper
		{
			
			static inline constexpr void call(Iter& it, Distance n)
			{
				while (n)
				{
					++it;
					--n;
				}
			}
		};
	}
	
	
	
	
	template <class Iter>
	typename iterator_traits<Iter>::difference_type distance(Iter first, Iter last)
	{
		return detail::distance_helper<Iter, typename iterator_traits<Iter>::iterator_category>::call(first, last);
	}
	
	template <class Iter, class Distance>
	void advance(Iter& it, Distance dist)
	{
		detail::advance_helper<Iter, Distance, typename iterator_traits<Iter>::iterator_category>::call(it, dist);
	}
	
	
}
#include "streambuf"
#endif