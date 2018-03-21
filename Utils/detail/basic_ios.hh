#ifndef INCLUDED_BASIC_IOS_HH
#define INCLUDED_BASIC_IOS_HH

#include "ios_base.hh"
#include <Utils/Char_Traits.hh>

namespace Utils {
	
	template <class T, class G>
	class basic_streambuf;
	
	template <class, class>
	class basic_ostream;
	
	template <class T, class Traits = Char_Traits<T>>
	class basic_ios : public ios_base
	{
		public:
		typedef T char_type;
		typedef typename Traits::int_type int_type;
		typedef typename Traits::pos_type pos_type;
		typedef typename Traits::off_type off_type;
		typedef Traits traits_type;
		
		
		private:
		basic_streambuf<T, Traits>* _streambuf;
		ios_base::iostate _state;
		char_type _fillC;
		
		
		protected:
		basic_ios();
		
		void init(basic_streambuf<T,Traits>* sb);
		void move(basic_ios& rhs);
		void move(basic_ios&& rhs);
		void swap(basic_ios& rhs) noexcept;
		void set_rdbuf(basic_streambuf<T, Traits>* sb);
		
		public:
		basic_ios(const basic_ios&) = delete;
		explicit basic_ios(basic_streambuf<T, Traits>*);
		
		virtual ~basic_ios() = default;
		
		
		explicit operator bool() const;
		bool operator!() const;
		iostate rdstate() const;
		void clear(iostate state = goodbit);
		void setstate(iostate state);
		bool good() const;
		bool eof() const;
		bool fail() const;
		bool bad() const;
		
		
		iostate exceptions() const;
		void exceptions(iostate except);
		
		
		
		
		basic_ostream<T,Traits>* tie() const;
		basic_ostream<T, Traits>* tie(basic_ostream<T, Traits>* tiestr);
		basic_streambuf<T, Traits>* rdbuf() const
		{ return _streambuf; }
		basic_streambuf<T, Traits>* rdbuf(basic_streambuf<T, Traits>* sb);
		basic_ios& copyfmt(const basic_ios& rhs);
		char_type fill() const
		{ return _fillC; }
		char_type fill(char_type ch)
		{
			char_type old = _fillC;
			_fillC = ch;
			return old;
		}
		locale imbue(const locale& loc);
		char narrow(char_type c, char dfault) const;
		char_type widen(char c) const;
		
		
		
		basic_ios& operator=(const basic_ios&) = delete;
		
	};
	
	typedef basic_ios<char> ios;
	typedef basic_ios<wchar_t> wios;
	
	
	
	
}


#endif


#include "basic_ios.hpp"