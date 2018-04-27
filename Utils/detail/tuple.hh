#ifndef INCLUDED_TUPLE_HH
#define INCLUDED_TUPLE_HH

#include <Common.h>
#include <Utils/utility>
#include "bits/decay.hh"

namespace Utils
{
	template <class...>
	class tuple;
	
	
	
	namespace detail
	{
		template <class...>
		class tuple_base;
		
		template <class T, class... Y>
		class tuple_base<T, Y...> : public tuple_base<Y...>
		{
			protected:
			
			public:
			typedef T type;
			T item;
			
			constexpr tuple_base() : tuple_base<Y...>(), item()
			{
				
			}
			
			tuple_base(T&& t, Y&&... y) : tuple_base<Y...>(forward<Y>(y)...), item(forward<T>(t))
			{
				
			}
			
			tuple_base(const T& t, const Y&... y) : tuple_base<Y...>(forward<Y>(y)...), item(forward<T>(t))
			{
				
			}
			
			template <class A, class... Args>
			tuple_base(A a, Args... args) : tuple_base<Y...>(forward<Args>(args)...), item(forward<A>(a))
			{
				
			}
		};
		
		template <class T>
		class tuple_base<T>
		{
			protected:
			
			public:
			typedef T type;
			T item;
			
			constexpr tuple_base() : item()
			{
				
			}
			
			tuple_base(T&& t) : item(forward<T>(t))
			{
				
			}
			
			tuple_base(const T& t) : item(t)
			{
				
			}
			
			template <class... Args>
			tuple_base(Args... args) : item(forward<Args>(args)...)
			{
				
			}
		};
		
		
		
		
		
		template <size_t N, class T, class... Y>
		struct type_at
		{
			typedef typename type_at<N-1, Y...>::type type;
		};
		
		template <class T, class... Y>
		struct type_at<0, T, Y...>
		{
			typedef T type;
		};
		
		
		
		template <size_t N, class T, class...Types>
		struct tuple_get_helper : public tuple_get_helper<N-1, Types...>
		{};
		
		
		template <class T, class...Types>
		struct tuple_get_helper<0, T, Types...>
		{
			constexpr static T& call(tuple_base<T, Types...>& b)
			{
				return b.item;
			}
			
			constexpr static const T& call(const tuple_base<T, Types...>& b)
			{
				return b.item;
			}
		};
		
	}
	
	
	template <class... Y>
	class tuple
	{
		protected:
		detail::tuple_base<Y...> base;
		public:
		
		template <class... Args>
		tuple(Args&&... args) : base(forward<Args>(args)...)
		{
			
		}
		
		template <size_t, class>
		friend struct tuple_element;
	};
	
	template <>
	class tuple<>
	{
		protected:
		
		public:
		
		constexpr tuple()
		{
			
		}
		
		template <size_t, class>
		friend struct tuple_element;
	};
	
	
	
	template <size_t N, class>
	struct tuple_element;
	
	template <size_t N, class T, class... Types>
	struct tuple_element<N, tuple<T, Types...>>
	{
		typedef typename detail::type_at<N, T, Types...>::type type;
		
		constexpr static type& get(tuple<T, Types...>& tup)
		{
			return detail::tuple_get_helper<N, T, Types...>::call(tup.base);
		}
		
		constexpr static const type& get(const tuple<T, Types...>& tup)
		{
			return detail::tuple_get_helper<N, T, Types...>::call(tup.base);
		}
	};
	
	
	
	template <size_t N, class... Types>
	constexpr typename tuple_element<N, tuple<Types...>>::type& get(tuple<Types...>&) noexcept;
	template <size_t N, class... Types>
	constexpr const typename tuple_element<N, tuple<Types...>>::type& get(const tuple<Types...>&) noexcept;
	
	template <class... Types>
	constexpr tuple<typename decay<Types>::type...> make_tuple(Types&&... a)
	{
		return tuple<typename decay<Types>::type...>(forward<Types&&>(a)...);
	}
}

#endif