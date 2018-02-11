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
	void basic_ios<T, Traits>::setstate(iostate st)
	{
		_state = st;
	}
	
	template <class T, class Traits>
	bool basic_ios<T, Traits>::good() const
	{
		return _state == goodbit;
	}
}


#endif