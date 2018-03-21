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
		/*const ios_base::fmtflags __fmt = this->flags() & ios_base::boolapha;
		if (__fmt | 0x0)
		{*/
			_insert(n);/*
		}
		else
		{
			
		}*/
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
		_insert(n);
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(unsigned long long n)
	{
		_insert(n);
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(float f)
	{
		_insert(f);
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(double f)
	{
		_insert(f);
		return *this;
	}
	
    template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(long double f)
	{
		_insert(f);
		return *this;
	}
	
	template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(const char_type* str)
	{
		
		sentry __cerb(*this);
		if (__cerb)
		{
			ios_base::iostate __err = ios_base::goodbit;
			
			auto len = traits_type::length(str);
			if (this->rdbuf()->sputn(str, len) != len)
			{
				__err |= ios_base::badbit;
			}
			
			
			if (__err)
			{
				this->setstate(__err);
			}
		}
		return *this;
	}
	
	template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::operator<<(const void* ptr)
	{
		static_assert(sizeof(addr_t) == sizeof(void*));
		
		sentry __cerb(*this);
		
		if (__cerb)
		{
			ios_base::iostate __err = ios_base::goodbit;
			
			if (has_facet<_nput_t>(this->getloc()))
			{
			const auto& np = use_facet<_nput_t>(this->getloc());
			if (np.put(*this, *this, ios_base::hex | ios_base::showbase, this->fill(), (addr_t)ptr).failed())
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
		
		return *this;
	}
	
	
	
	
	
	template <class T, class Traits>
	basic_ostream<T, Traits>& basic_ostream<T, Traits>::flush()
	{
		sentry __cerb(*this);
		
		if (__cerb)
		{
			ios_base::iostate __err = this->rdstate();
			if (this->rdbuf()->pubsync() != 0)
			{
				this->setstate(this->rdstate() | ios_base::badbit);
			}
		}
		return *this;
	}
}


#endif