#ifndef INCLUDED_ISTREAM_HPP
#define INCLUDED_ISTREAM_HPP

#include "istream.hh"

namespace Utils
{
    template <class T, class Traits>
    basic_istream<T, Traits>::basic_istream(basic_streambuf<T, Traits>* buf) : basic_ios<T, Traits>()
    {
        basic_ios<T, Traits>::init(buf);
    }










    template <class T, class Traits>
    auto basic_istream<T, Traits>::read(char_type* s, streamsize n) -> basic_istream&
    {
        sentry __cerb(*this);

        if (__cerb)
        {
            ios_base::iostate __err = this->rdstate();
            if (this->rdbuf()->sgetn(s, n) != n)
            {
                __err |= ios_base::badbit;
            }
            this->setstate(__err);
        }
        return *this;
    }
    
    
    template <class T, class Traits>
    basic_istream<T, Traits>& basic_istream<T, Traits>::operator>>(basic_streambuf<T, Traits>* sbuf)
    {
    	ios_base::iostate err = ios_base::iostate(ios_base::goodbit);
    	sentry cerb(*this);
    	if (cerb && sbuf)
    	{
    		__try
    		{
    			if (!__copy_streambufs(this->rdbuf(), sbuf))
    			{
    				err |= ios_base::failbit;
    			}
    		}
    		__catch(...)
    		{
    			this->setstate(ios_base::failbit);
    		}
    	}
    	else if (!sbuf)
    	{
    		err |= ios_base::failbit;
    	}
    	
    	if (err)
    	{
    		this->setstate(err);
    	}
    	
    	return *this;
    }
}

#endif