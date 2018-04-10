#ifndef INCLUDED_ISTREAM_HH
#define INCLUDED_ISTREAM_HH

#include "basic_ios.hh"

namespace Utils {
	
	template <class T, class Traits = Char_Traits<T>>
	class basic_istream : virtual public basic_ios<T, Traits>
	{
		private:
		
		protected:
		
		basic_istream(const basic_istream&) = delete;
		basic_istream(basic_istream&&);
		
		basic_istream& operator=(const basic_istream&) = delete;
		basic_istream& operator=(basic_istream&&);
		void swap(basic_istream&);
		
		public:
		#define BASETYPE(X) typedef typename basic_ios<T, Traits>:: X X
		
		BASETYPE(char_type);
		BASETYPE(int_type);
		BASETYPE(pos_type);
		BASETYPE(off_type);
		BASETYPE(traits_type);
		
		#undef BASETYPE
		
		explicit basic_istream(basic_streambuf<T, Traits>* sb);
		virtual ~basic_istream() = default;

        class sentry
		{
			private:
			bool _good;
			protected:
			basic_istream& is;
			public:
			explicit sentry(basic_istream& is) : _good(true), is(is)
			{
				if (!is.good())
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
		
		
		
		basic_istream<T, Traits>& operator>>(
        basic_istream<T, Traits>& (*pf)(basic_istream<T, Traits>&));
    basic_istream<T, Traits>& operator>>(
        basic_ios<T, Traits>& (*pf)(basic_ios<T, Traits>&));
    basic_istream<T, Traits>& operator>>(
        ios_base& (*pf)(ios_base&));
        
        basic_istream<T, Traits>& operator>>(bool& n);
        basic_istream<T, Traits>& operator>>(short& n);
        basic_istream<T, Traits>& operator>>(unsigned short& n);
        basic_istream<T, Traits>& operator>>(int& n);
        basic_istream<T, Traits>& operator>>(unsigned int& n);
        basic_istream<T, Traits>& operator>>(long& n);
        basic_istream<T, Traits>& operator>>(unsigned long& n);
        basic_istream<T, Traits>& operator>>(long long& n);
        basic_istream<T, Traits>& operator>>(unsigned long long& n);
        basic_istream<T, Traits>& operator>>(float& f);
        basic_istream<T, Traits>& operator>>(double& f);
        basic_istream<T, Traits>& operator>>(long double& f);
        basic_istream<T, Traits>& operator>>(void*& p);
        basic_istream<T, Traits>& operator>>(
        	basic_streambuf<T, Traits>* sb
        );
        
        
        
        streamsize gcount() const;
        int_type get();
        basic_istream<T, Traits>& get(char_type& c);
        basic_istream<T, Traits>& get(char_type* s, streamsize n);
        basic_istream<T, Traits>& get(char_type* s, streamsize n, char_type delim);
        basic_istream<T, Traits>& get(basic_streambuf<char_type, Traits>& sb);
        basic_istream<T, Traits>& get(basic_streambuf<char_type, Traits>& sb,
                                     char_type delim);
 
    basic_istream<T, Traits>& getline(char_type* s, streamsize n);
    basic_istream<T, Traits>& getline(char_type* s, streamsize n,
                                         char_type delim);
 
 
    basic_istream<T, Traits>& ignore(
        streamsize n = 1, int_type delim = Traits::eof());
    int_type peek();
    basic_istream<T, Traits>& read(char_type* s, streamsize n);
    streamsize readsome(char_type* s, streamsize n);
    basic_istream<T, Traits>& putback(char_type c);
    basic_istream<T, Traits>& unget();
    int sync();
 
    pos_type tellg();
    basic_istream<T, Traits>& seekg(pos_type);
    basic_istream<T, Traits>& seekg(off_type, ios_base::seekdir);
	};
}

#endif