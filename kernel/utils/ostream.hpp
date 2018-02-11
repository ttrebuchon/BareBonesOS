#ifndef INCLUDED_OSTREAM_HPP
#define INCLUDED_OSTREAM_HPP

namespace Utils {
	
	template <class T, class Traits>
	basic_ostream<T, Traits>::basic_ostream(basic_streambuf<T, Traits>* buf) : basic_ios<T, Traits>()
	{
		basic_ios<T, Traits>::init(buf);
	}
	
	
	
	
	
	
	template <class T, class Traits>
	template <class Value_t>
	void basic_ostream<T, Traits>::_insert(Value_t v)
	{
		sentry __cerb(*this);
		
		if (__cerb)
		{
			ios_base::iostate __err = ios_base::goodbit;
			//TODO
			if (has_facet<_nput_t>(this->getloc()))
			{
			const auto& np = use_facet<_nput_t>(this->getloc());
			if (np.put(*this, *this, this->fill(), v).failed())
			{
				__err |= ios_base::badbit;
			}
			}
			else
			{
				__err |= ios_base::badbit;
			}
			
			if (__err)
			{
				this->setstate(__err);
			}
		}
	}
	
	
	
	template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(bool n)
	{
		
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(short n)
	{
		const ios_base::fmtflags __fmt = this->flags() & ios_base::basefield;
		if (__fmt == ios_base::oct || __fmt == ios_base::hex)
		{
			_insert(static_cast<long>(static_cast<unsigned short>(n)));
		}
		else
		{
			_insert(static_cast<long>(n));
		}
		
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(unsigned short n)
	{
		_insert(static_cast<unsigned long>(n));
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(int n)
	{
		const ios_base::fmtflags __fmt = this->flags() & ios_base::basefield;
		if (__fmt == ios_base::oct || __fmt == ios_base::hex)
		{
			_insert(static_cast<long>(static_cast<unsigned int>(n)));
		}
		else
		{
			_insert(static_cast<long>(n));
		}
		
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(unsigned int n)
	{
		_insert(static_cast<unsigned long>(n));
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(long n)
	{
		_insert(n);
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(unsigned long n)
	{
		_insert(n);
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(long long n)
	{
		
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(unsigned long long n)
	{
		
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(float f)
	{
		
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(double f)
	{
		
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(long double f)
	{
		
		return *this;
	}
}


#endif