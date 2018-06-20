#ifndef INCLUDED_OSTREAM_HH
#define INCLUDED_OSTREAM_HH
#define INCLUDED_OSTREAM_HH_IN_PROGRESS

#include "detail/basic_ios.hh"
#include "detail/locale.hh"

namespace Utils {
	
	template <class T, class Traits = Char_Traits<T>>
	class basic_ostream : virtual public basic_ios<T, Traits>
	{
		private:
		
		template <class Value_t>
		void _insert(Value_t);
		
		typedef num_put<T, ostreambuf_iterator<T, Traits>> _nput_t;
		
		protected:
		
		public:
		
		#define BASETYPE(X) typedef typename basic_ios<T, Traits>:: X X
		
		BASETYPE(char_type);
		BASETYPE(int_type);
		BASETYPE(pos_type);
		BASETYPE(off_type);
		BASETYPE(traits_type);
		
		#undef BASETYPE
		
		
		explicit basic_ostream(basic_streambuf<T, Traits>*);
		virtual ~basic_ostream() = default;
		
		
		class sentry
		{
			private:
			bool _good;
			protected:
			basic_ostream& os;
			public:
			explicit sentry(basic_ostream& os) : _good(true), os(os)
			{
				if (!os.good())
				{
					_good = false;
					return;
				}
			}
			sentry& operator=(const sentry&) = delete;
			
			operator bool()
			{
				if (!_good)
				{
					return false;
				}
				
				
				return true;
			}
		};
		
		
		
		basic_ostream<T, Traits>& operator<<(basic_ostream<T, Traits>& (*pf)(basic_ostream<T, Traits>&));
		basic_ostream<T, Traits>& operator<<(basic_ios<T, Traits>& (*pf)(basic_ios<T, Traits>&));
        basic_ostream<T, Traits>& operator<<(ios_base& (*pf)(ios_base&));
        
        basic_ostream<T, Traits>& operator<<(bool n);
        basic_ostream<T, Traits>& operator<<(short n);
        basic_ostream<T, Traits>& operator<<(unsigned short n);
        basic_ostream<T, Traits>& operator<<(int n);
        basic_ostream<T, Traits>& operator<<(unsigned int n);
        basic_ostream<T, Traits>& operator<<(long n);
        basic_ostream<T, Traits>& operator<<(unsigned long n);
        basic_ostream<T, Traits>& operator<<(long long n);
        basic_ostream<T, Traits>& operator<<(unsigned long long n);
        basic_ostream<T, Traits>& operator<<(float f);
        basic_ostream<T, Traits>& operator<<(double f);
        basic_ostream<T, Traits>& operator<<(long double f);
        basic_ostream& operator<<(const char_type*);
        
        basic_ostream<T, Traits>& operator<<(const void* p);
        basic_ostream<T, Traits>& operator<<(basic_streambuf<char_type, Traits>* sb);
        
        
		basic_ostream<T, Traits>& put(char_type);
		basic_ostream<T, Traits>& write(const char_type*, streamsize);
		basic_ostream<T, Traits>& flush();
	};
	
	typedef basic_ostream<char> ostream;
	typedef basic_ostream<wchar_t> wostream;
	
	template <class Traits>
	basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>&, const signed char*);
	
	template <class Traits>
	basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>& os, const unsigned char* cstr)
	{
		return (os << (const char*)cstr);
	}


	template <class T, class Traits>
	inline basic_ostream<T, Traits>& flush(basic_ostream<T, Traits>& os)
	{
		return os.flush();
	}


	template <class T, class Traits>
	inline basic_ostream<T, Traits>& endl(basic_ostream<T, Traits>& os)
	{
		return flush(os.put(os.widen('\n')));
	}
}

#ifndef TESTING

	namespace std
	{
		using Utils::flush;
		using Utils::endl;
	}

#endif

#undef INCLUDED_OSTREAM_HH_IN_PROGRESS

#endif

// #include "ostream.hpp"