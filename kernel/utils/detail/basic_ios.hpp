#ifndef INCLUDED_BASIC_IOS_HPP
#define INCLUDED_BASIC_IOS_HPP

namespace Utils {
	
	
	template <class T, class Traits>
	basic_ios<T, Traits>::basic_ios() : _streambuf(nullptr), _state(ios_base::goodbit)
	{
		
	}
	
	template <class T, class Traits>
	void basic_ios<T, Traits>::init(basic_streambuf<T, Traits>* buf)
	{
		//ios_base::init();
		
		this->_streambuf = buf;
	}
	
	template <class T, class Traits>
	ios_base::iostate basic_ios<T, Traits>::rdstate() const
	{
		return _state;
	}
	
	template <class T, class Traits>
	void basic_ios<T, Traits>::setstate(iostate st)
	{
		_state = st;
	}
	
	template <class T, class Traits>
	bool basic_ios<T, Traits>::good() const
	{
		return _state == goodbit;
	}
	
	
	
	
	
	
	
	
	
	
	template <class T, class Traits>
	basic_streambuf<T, Traits>* basic_ios<T, Traits>::rdbuf(basic_streambuf<T, Traits>* sb)
	{
		auto old = _streambuf;
		_streambuf = sb;
		return old;
	}
}


#endif