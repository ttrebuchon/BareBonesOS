#ifndef INCLUDED_STREAMBUF_HPP
#define INCLUDED_STREAMBUF_HPP

#include "streambuf.hh"

namespace Utils {
	
	template <class T, class Traits>
	basic_streambuf<T, Traits>::basic_streambuf() : _pstart(nullptr), _pptr(nullptr), _pend(nullptr), _gstart(nullptr), _gptr(nullptr), _gend(nullptr)
	{
		
	}
	
	
	
	
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::eback() const
	{
		return _gstart;
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::gptr() const
	{
		return _gptr;
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::egptr() const
	{
		return _gend;
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::gbump(int n)
	{
		
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::setg(char_type* gbeg, char_type* gnext, char_type* gend)
	{
		_gstart = gbeg;
		_gptr = gnext;
		_gend = gend;
	}
	
	
	
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::pbase() const
	{
		return _pstart;
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::pptr() const
	{
		return _pptr;
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::epptr() const
	{
		return _pend;
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::pbump(int n)
	{
		_pptr += n;
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::setp(char_type* pbeg, char_type* pend)
	{
		_pstart = pbeg;
		_pend = pend;
		_pptr = pbeg;
	}
	
	
	
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::imbue(const locale&)
	{
		
	}
	
	
	
	template <class T, class Traits>
	basic_streambuf<T, Traits>* basic_streambuf<T, Traits>::setbuf(char_type* ptr, streamsize n)
	{
		return this;
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::pos_type basic_streambuf<T, Traits>::seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which)
	{
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::pos_type basic_streambuf<T, Traits>::seekpos(pos_type pos, ios_base::openmode which)
	{
		
	}
	
	template <class T, class Traits>
	int basic_streambuf<T, Traits>::sync()
	{
		return 0;
	}
	
	template <class T, class Traits>
	streamsize basic_streambuf<T, Traits>::showmanyc()
	{
		
	}
	
	template <class T, class Traits>
	streamsize basic_streambuf<T, Traits>::xsgetn(char_type* s, streamsize n)
	{
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::int_type basic_streambuf<T, Traits>::underflow()
	{
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::int_type basic_streambuf<T, Traits>::uflow()
	{
		
	}
	
	
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::int_type basic_streambuf<T, Traits>::pbackfail(int_type c)
	{
		
	}
	
	
	
	template <class T, class Traits>
	streamsize basic_streambuf<T, Traits>::xsputn(const char_type* s, streamsize n)
	{
		streamsize ret = 0;
		while (ret < n)
		{
			const streamsize buf_len = epptr() - pptr();
			if (buf_len > 0)
			{
				const streamsize remain = n - ret;
				const streamsize len = (buf_len < remain ? buf_len : remain);
				traits_type::copy(pptr(), s, len);
				ret += len;
				s += len;
				pbump(len);
			}
			
			if (ret < n)
			{
				int_type c = overflow(traits_type::to_int_type(*s));
				if (!traits_type::eq_int_type(c, traits_type::eof()))
				{
					++ret;
					++s;
				}
				else
				{
					break;
				}
			}
		}
		return ret;
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::int_type basic_streambuf<T, Traits>::overflow(int_type c)
	{
		return traits_type::eof();
	}
	
	
	
	
	
	
	
	template <class T, class Traits>
	locale basic_streambuf<T, Traits>::pubimbue(const locale& loc)
	{
		auto old = _loc;
		_loc = loc;
		imbue(loc);
		return old;
	}
	
	
	template <class T, class Traits>
	int basic_streambuf<T, Traits>::pubsync()
	{
		return sync();
	}
	
	template <class T, class Traits>
	basic_streambuf<T, Traits>* basic_streambuf<T, Traits>::pubsetbuf(char_type* s, streamsize n)
	{
		return setbuf(s, n);
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::pos_type basic_streambuf<T, Traits>::pubseekoff(off_type off, ios_base::seekdir way, ios_base::openmode which)
	{
		return seekoff(off, way, which);
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::pos_type basic_streambuf<T, Traits>::pubseekpos(pos_type sp, ios_base::openmode which)
	{
		return seekpos(sp, which);
	}
	
	
	
	
	
	
	
	
	template <class T, class Traits>
	auto basic_streambuf<T, Traits>::sputc(char_type c) -> int_type
	{
		if (this->pptr() < epptr())
		{
			*pptr() = c;
			pbump(1);
			return traits_type::to_int_type(c);
		}
		else
		{
			return overflow(traits_type::to_int_type(c));
		}
	}
	
	template <class T, class Traits>
	auto basic_streambuf<T, Traits>::sputn(const char_type* c, streamsize n) -> streamsize
	{
		return this->xsputn(c, n);
	}
}

#endif