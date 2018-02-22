#ifndef INCLUDED_ITERATOR_HH
#define INCLUDED_ITERATOR_HH

#include "Char_Traits.hh"


namespace Utils {
	
	template <class T, class Traits>
	class basic_ostream;
	template <class T, class Traits>
	class basic_streambuf;
	
	template <class T, class Traits = Char_Traits<T>>
	class ostreambuf_iterator
	{
		public:
		typedef basic_streambuf<T, Traits> streambuf_type;
		typedef basic_ostream<T, Traits> ostream_type;
		
		private:
		streambuf_type* buf;
		bool failbit;
		
		void _put(const T*, size_t len);
		
		public:
		
		ostreambuf_iterator(streambuf_type* b) : buf(b), failbit(false)
		{
			
		}
		
		ostreambuf_iterator(ostream_type& o) : ostreambuf_iterator(o.rdbuf())
		{
			
		}
		
		
		
		bool failed() const
		{
			return failbit;
		}
		
		
		template <class G, class X>
		friend X __write(X, const G*, size_t);
		template <class G, class X>
		friend X __write(X, G);
		template <class G>
		friend ostreambuf_iterator<G> __write(ostreambuf_iterator<G>, const G*, size_t);
		template <class G>
		friend ostreambuf_iterator<G> __write(ostreambuf_iterator<G>, G);
	};
	
}
#endif