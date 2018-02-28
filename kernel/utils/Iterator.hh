#ifndef INCLUDED_ITERATOR_HH
#define INCLUDED_ITERATOR_HH

#include "Char_Traits.hh"
#include "iterator_traits.hh"


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
	
	
	template <class _It, class C>
	class normal_iterator
	{
		private:
		
		protected:
		_It _current;
		
		public:
		typedef _It iterator_type;
		typedef typename iterator_traits<_It>::iterator_category iterator_category;
		typedef typename iterator_traits<_It>::value_type value_type;
		typedef typename iterator_traits<_It>::difference_type difference_type;
		typedef typename iterator_traits<_It>::reference reference;
		typedef typename iterator_traits<_It>::pointer pointer;
		
		
		
		
		
		normal_iterator() : _current(_It())
		{
			
		}
		
		explicit normal_iterator(const _It& _i) : _current(_i)
		{
			
		}
		
		
		normal_iterator& operator++()
		{
			++_current;
			return *this;
		}
		
		normal_iterator operator++(int)
		{
			return normal_iterator(_current++);
		}
		
		reference operator*() const
		{
			return *_current;
		}
		
		pointer operator->() const
		{
			return _current.operator->();
		}
		
		
		
		const _It& base() const
		{
			return _current;
		}
	};
	
	template <class _ItL, class _ItR, class C>
	bool operator==(const normal_iterator<_ItL, C>& lhs, const normal_iterator<_ItR, C>& rhs)
	{
		return lhs.base() == rhs.base();
	}
	
	template <class _ItL, class _ItR, class C>
	bool operator!=(const normal_iterator<_ItL, C>& lhs, const normal_iterator<_ItR, C>& rhs)
	{
		return lhs.base() != rhs.base();
	}
	
}
#endif