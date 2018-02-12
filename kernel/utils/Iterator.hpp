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
	
	
}
#include "streambuf"
#endif