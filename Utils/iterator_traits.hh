#ifndef INCLUDED_ITERATOR_TRAITS_HH
#define INCLUDED_ITERATOR_TRAITS_HH

#include "utility"
#include <Utils/void_t.hh>

namespace Utils
{
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag
	{};
	struct bidirectional_iterator_tag : public forward_iterator_tag
	{};
	struct random_access_iterator_tag : public bidirectional_iterator_tag
	{};
	
	/*namespace detail
	{
	namespace Iter_Tags
	{
		template <class It, bool output, bool input>
		struct _iter_category;
		
		template <class It>
		struct _iter_category<It, true, false>
		{
			typedef output_iterator_tag category;
		};
		
		
		
		template <class It, class = void>
		struct is_out_iter
		{
			constexpr static bool value = false;
		};
		
		template <class It>
		struct is_out_iter<It, void_t<decltype(*declval<It>())>>
		{
			constexpr static bool value = false;
		};
		
		
		
		
		
		
		
		
		
		
		
		
		template <class It, class V = void>
		struct is_input_iter
		{
			constexpr static bool input_value = false;
			constexpr static bool value = false;
		};
		
		template <class It>
		struct is_input_iter<It, void_t<decltype(++declval<It>()), decltype(*declval<It>())>>
		{
			
			constexpr static bool input_value = true;
			typedef input_iterator_tag type;
			constexpr static bool value = true;
		};
		
		
		
		template <class It, class V = void>
		struct is_forward_iter
		{
			constexpr static bool input_value = is_input_iter<It>::input_value;
			typedef typename is_input_iter<It>::type type;
			constexpr static bool value = false;
		};
		
		template <class It>
		struct is_forward_iter<It, void_t<decltype(declval<It>()++)>>
		{
			
			constexpr static bool input_value = true;
			typedef forward_iterator_tag type;
			constexpr static bool value = true;
		};
		
		
		
		
		template <class It, class V = void>
		struct is_bidir_iter
		{
			constexpr static bool input_value = is_forward_iter<It>::input_value;
			typedef typename is_forward_iter<It>::type type;
			constexpr static bool value = false;
		};
		
		template <class It>
		struct is_bidir_iter<It, void_t<decltype(++declval<It>()), decltype(--declval<It>())>>
		{
			
			constexpr static bool input_value = true;
			typedef bidirectional_iterator_tag type;
			constexpr static bool value = true;
		};
		
		
		template <class It, class V = void>
		struct is_ra_iter
		{
			constexpr static bool input_value = is_bidir_iter<It>::input_value;
			typedef typename is_bidir_iter<It>::type type;
			constexpr static bool value = false;
		};
		
		template <class It>
		struct is_ra_iter<It, void_t<decltype(declval<It>()[4])>>
		{
			
			constexpr static bool input_value = true;
			typedef random_access_iterator_tag type;
			constexpr static bool value = true;
		};
		
		template <class It>
		struct is_in_iter
		{
			constexpr static bool value = is_ra_iter<It>::input_value;
		};
		
		template <class It, bool output>
		struct _iter_category<It, output, true>
		{
			typedef typename is_ra_iter<It>::type category;
		};
		
		
		
		
		template <class It>
		struct iter_category
		{
			typedef typename _iter_category<It, is_out_iter<It>::value, is_in_iter<It>::value>::category type;
		};
		
		
	}
	}
	
	
	
	template <class It>
	struct iterator_traits
	{
		typedef typename remove_ref<decltype(*declval<It>())>::type value_type;
		typedef decltype(declval<It>() - declval<It>()) difference_type;
		typedef typename detail::Iter_Tags::iter_category<It>::type iterator_category;
		typedef value_type* pointer;
		typedef value_type& reference;
	};*/
	
	template <class It>
	struct iterator_traits
	{
		typedef typename It::value_type value_type;
		typedef typename It::difference_type difference_type;
		typedef typename It::iterator_category iterator_category;
		typedef typename It::pointer pointer;
		typedef typename It::reference reference;
	};
	
	template <class T>
	struct iterator_traits<T*>
	{
		typedef T value_type;
		typedef decltype((T*)nullptr - (T*)nullptr) difference_type;
		typedef random_access_iterator_tag iterator_category;
		typedef T* pointer;
		typedef T& reference;
	};
	
	template <class T>
	struct iterator_traits<const T*>
	{
		typedef T value_type;
		typedef decltype((T*)nullptr - (T*)nullptr) difference_type;
		typedef random_access_iterator_tag iterator_category;
		typedef const T* pointer;
		typedef const T& reference;
	};
	
	
	
	
	
	template <class It>
	class reverse_iterator
	{
		public:
		typedef It iterator_type;
		typedef typename iterator_traits<It>::value_type value_type;
		typedef typename iterator_traits<It>::difference_type difference_type;
		typedef typename iterator_traits<It>::pointer pointer;
		typedef typename iterator_traits<It>::reference reference;
		typedef typename iterator_traits<It>::iterator_category iterator_category;
		
		protected:
		iterator_type current;
		
		
		public:
		constexpr reverse_iterator() : current()
		{}
		constexpr explicit reverse_iterator(iterator_type i) : current(i)
		{}
		template <class U>
		constexpr reverse_iterator(const reverse_iterator<U>& other) : current(other.current)
		{}
		
		
		constexpr iterator_type base() const
		{ return current; }
		
		
		
		template <class U>
		constexpr reverse_iterator& operator=(const reverse_iterator<U>& other)
		{
			current = other.current;
		}
		
		constexpr reference operator*() const
		{
			return *current;
		}
		
		constexpr pointer operator->() const
		{
			return current.operator->();
		}
		
		constexpr auto operator[](difference_type n) const
		{
			return current[-n-1];
		}
		
		constexpr reverse_iterator& operator++()
		{
			current.operator--();
			return *this;
		}
		
		constexpr reverse_iterator& operator--()
		{
			current.operator++();
			return *this;
		}
		
		constexpr reverse_iterator operator++(int)
		{
			reverse_iterator it2(current);
			current.operator--(0);
			return it2;
		}
		
		constexpr reverse_iterator operator--(int)
		{
			reverse_iterator it2(current);
			current.operator++(0);
			return it2;
		}
		
		constexpr reverse_iterator operator+(difference_type d) const
		{
			return reverse_iterator(current - d);
		}
		
		constexpr reverse_iterator operator-(difference_type d) const
		{
			return reverse_iterator(current + d);
		}
		
		constexpr reverse_iterator& operator+=(difference_type d)
		{
			current -= d;
			return *this;
		}
		
		constexpr reverse_iterator& operator-=(difference_type d)
		{
			current += d;
			return *this;
		}
	};
	
	
	template <class I1, class I2>
	constexpr bool operator==(const reverse_iterator<I1>& it1, const reverse_iterator<I2>& it2)
	{
		return it1.base() == it2.base();
	}
	
	template <class I1, class I2>
	constexpr bool operator!=(const reverse_iterator<I1>& it1, const reverse_iterator<I2>& it2)
	{
		return it1.base() != it2.base();
	}
	
	template <class I1, class I2>
	constexpr bool operator>(const reverse_iterator<I1>& it1, const reverse_iterator<I2>& it2)
	{
		return it1.base() > it2.base();
	}
	
	template <class I1, class I2>
	constexpr bool operator<(const reverse_iterator<I1>& it1, const reverse_iterator<I2>& it2)
	{
		return it1.base() < it2.base();
	}
	
	template <class I1, class I2>
	constexpr bool operator>=(const reverse_iterator<I1>& it1, const reverse_iterator<I2>& it2)
	{
		return it1.base() >= it2.base();
	}
	
	template <class I1, class I2>
	constexpr bool operator<=(const reverse_iterator<I1>& it1, const reverse_iterator<I2>& it2)
	{
		return it1.base() <= it2.base();
	}
	
	template <class It>
	constexpr reverse_iterator<It> operator+(typename reverse_iterator<It>::difference_type n, const reverse_iterator<It>& rhs)
	{
		return rhs.operator+(n);
	}
	
	template <class I1, class I2>
	constexpr auto operator-(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs) -> decltype(rhs.base() - lhs.base())
	{
		return rhs.base() - lhs.base();
	}
}

#endif