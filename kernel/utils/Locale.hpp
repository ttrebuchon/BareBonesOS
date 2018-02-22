#ifndef INCLUDED_LOCALE_HPP
#define INCLUDED_LOCALE_HPP

#include "Locale.hh"
#include "detail/ios_base.hh"
#include "detail/num_put.hh"

namespace Utils {
	
	
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::bool_str(const bool v, iter_type& it, ios_base& io)
	{
		const auto flags = io.flags();
		if ((flags & ios_base::boolalpha) == 0)
		{
			long_long_str(static_cast<long>(v), it, io);
			return;
		}
		
		
		if (v)
		{
			it = __write(it, _trueStr, _trueStrLen);
		}
		else
		{
			it = __write(it, _falseStr, _falseStrLen);
		}
	}
	
	template <class T, class OutputIt>
	template <class N>
	void num_put<T, OutputIt>::numeric_str(N v, iter_type& it, ios_base& io)
	{
		numeric_str<N>(v, it, io, io.flags());
	}
	
	template <class T, class OutputIt>
	template <class N>
	void num_put<T, OutputIt>::numeric_str(N v, iter_type& it, ios_base& io, ios_base::fmtflags flags)
	{
		if ((flags & ios_base::basefield) == 0)
		{
			flags |= (io.flags() & ios_base::basefield);
		}
		if (v > 0 && (flags & ios_base::showpos))
		{
			it = __write<T, iter_type>(it, pos);
		}
		else if (v < 0)
		{
			it = __write<T, iter_type>(it, neg);
		}
		
		if (v == 0)
		{
			it = __write<T, iter_type>(it, digits[0]);
			return;
		}
		
		
		int base;
		if (((ios_base::basefield & flags) != ios_base::hex) && ((ios_base::basefield & flags) != ios_base::oct))
		{
			base = 10;
		}
		else if ((ios_base::basefield & flags) == ios_base::oct)
		{
			base = 8;
			if (flags & ios_base::showbase)
			{
				it = __write(it, _octPrefix, _octPrefixLen);
			}
		}
		else
		{
			base = 16;
			if (flags & ios_base::showbase)
			{
				it = __write(it, _hexPrefix, _hexPrefixLen);
			}
		}
		
		kassert(base > 0);
		
		N div = 1;
		size_t digitCount = 1;
		while ((v / div) > (base-1))
		{
			div *= base;
			++digitCount;
		}
		
		T buf[digitCount];
		
		for (int i = 0; i < digitCount; ++i)
		{
			buf[i] = digits[v / div];
			v %= div;
			div /= base;
		}
		
		
		//kassert(false);
		it = __write<T, iter_type>(it, buf, digitCount);
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::long_long_str(const long long v, iter_type& it, ios_base& io)
	{
		numeric_str<long long>(v, it, io);
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::unsigned_long_long_str(const unsigned long long v, iter_type& it, ios_base& io)
	{
		numeric_str<unsigned long long>(v, it, io);
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::long_double_str(const long double v, iter_type& it, ios_base& io)
	{
		kassert(false);
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::ptr_str(const void* v, iter_type& it, ios_base& io)
	{
		kassert(false);
	}
	
	
	//With Flags:
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::bool_str(const bool v, iter_type& it, ios_base& io, ios_base::fmtflags flags)
	{
		if ((flags & ios_base::boolalpha) == 0)
		{
			long_long_str(static_cast<long>(v), it, io, flags);
			return;
		}
		
		
		if (v)
		{
			it = __write(it, _trueStr, _trueStrLen);
		}
		else
		{
			it = __write(it, _falseStr, _falseStrLen);
		}
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::long_long_str(const long long v, iter_type& it, ios_base& io, ios_base::fmtflags flags)
	{
		numeric_str<long long>(v, it, io, flags);
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::unsigned_long_long_str(const unsigned long long v, iter_type& it, ios_base& io, ios_base::fmtflags flags)
	{
		numeric_str<unsigned long long>(v, it, io, flags);
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::long_double_str(const long double v, iter_type& it, ios_base& io, ios_base::fmtflags flags)
	{
		kassert(false);
	}
	
	template <class T, class OutputIt>
	void num_put<T, OutputIt>::ptr_str(const void* v, iter_type& it, ios_base& io, ios_base::fmtflags flags)
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
	
	template <class T, class OutputIt>
	OutputIt __write(OutputIt s, T t)
	{
		s._put(&t, 1);
		return s;
	}
	
	template <class T, class OutputIt>
	OutputIt ___write(OutputIt s, T t)
	{
		s._put(&t, 1);
		return s;
	}
}
#endif