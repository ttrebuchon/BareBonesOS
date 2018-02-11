#ifndef INCLUDED_STREAMBUF_HPP
#define INCLUDED_STREAMBUF_HPP

namespace Utils {
	
	template <class T, class Traits>
	basic_streambuf<T, Traits>::basic_streambuf() : _pstart(nullptr), _pptr(nullptr), _pend(nullptr), _gstart(nullptr), _gptr(nullptr), _gend(nullptr)
	{
		
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
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::eback() const
	{
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::gptr() const
	{
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::egptr() const
	{
		
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::gbump(int n)
	{
		
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::setg(char_type* gbeg, char_type* gnext, char_type* gend)
	{
		
	}
	
	
	
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::pbase() const
	{
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::pptr() const
	{
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::char_type* basic_streambuf<T, Traits>::epptr() const
	{
		
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::pbump(int n)
	{
		
	}
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::setp(char_type* pbeg, char_type* pend)
	{
		
	}
	
	
	
	
	template <class T, class Traits>
	void basic_streambuf<T, Traits>::imbue(const locale&)
	{
		
	}
	
	
	
	template <class T, class Traits>
	basic_streambuf<T, Traits>* basic_streambuf<T, Traits>::setbuf(char_type* ptr, streamsize n)
	{
		
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
		
	}
	
	template <class T, class Traits>
	typename basic_streambuf<T, Traits>::int_type basic_streambuf<T, Traits>::overflow(int_type c)
	{
		
	}
}

#endif