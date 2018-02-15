#ifndef INCLUDED_LOCALE_HPP
#define INCLUDED_LOCALE_HPP

#include "Locale.hh"
#include "detail/ios_base.hh"

namespace Utils {
	
	
	
	template <class T, class OutputIt>
	const T* num_put<T, OutputIt>::bool_str(const bool v, size_t& len, ios_base& io)
	{
		const auto flags = io.flags();
		if ((flags & ios_base::boolalpha) == 0)
		{
			return long_long_str(static_cast<long>(v), len, io);
		}
		
		
		if (v)
		{
			len = _trueStrLen;
			return _trueStr;
		}
		else
		{
			len = _falseStrLen;
			return _falseStr;
		}
	}
	
	template <class T, class OutputIt>
	const T* num_put<T, OutputIt>::long_long_str(const long long v, size_t& len, ios_base& str)
	{
		kassert(false);
	}
	
	template <class T, class OutputIt>
	const T* num_put<T, OutputIt>::unsigned_long_long_str(const unsigned long long v, size_t& len, ios_base& str)
	{
		kassert(false);
	}
	
	template <class T, class OutputIt>
	const T* num_put<T, OutputIt>::long_double_str(const long double v, size_t& len, ios_base& str)
	{
		kassert(false);
	}
	
	template <class T, class OutputIt>
	const T* num_put<T, OutputIt>::ptr_str(const void* v, size_t& len, ios_base& str)
	{
		kassert(false);
	}
	
	template <class T>
	ostreambuf_iterator<T> __write(ostreambuf_iterator<T> s, const T* str, size_t len)
	{
		s._put(str, len);
		return s;
	}
	
	template <class T, class OutputIt>
	OutputIt __write(OutputIt s, const T* str, size_t len)
	{
		s._put(str, len);
		return s;
	}
}
#endif