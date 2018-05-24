/*#ifndef INCLUDED_TUPLE_HH
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
			
			constexpr tuple_base(T&& t) : item(forward<T>(t))
			{
				
			}
			
			constexpr tuple_base(const T& t) : item(t)
			{
				
			}
			
			template <class... Args>
			constexpr tuple_base(Args... args) : item(forward<Args>(args)...)
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
		
		constexpr tuple()
		{
			
		}
		
		explicit constexpr tuple(const Y&... vals) : base(forward<const Y&...>(vals)...)
		{
			
		}
		
		template <class... Args>
		explicit constexpr tuple(Args&&... args) : base(forward<Args>(args)...)
		{
			
		}
		
		template <class... Args>
		constexpr tuple(const tuple<Args...>&);
		
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

#endif*/









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
			
			constexpr tuple_base(T&& t) : item(forward<T>(t))
			{
				
			}
			
			constexpr tuple_base(const T& t) : item(t)
			{
				
			}
			
			template <class... Args>
			constexpr tuple_base(Args... args) : item(forward<Args>(args)...)
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
		
		template <template <class...> class Result, unsigned N, class... Args>
		struct type_subsequence;
		
		template <template <class...> class Result, unsigned N, class T, class... Args>
		struct type_subsequence<Result, N, T, Args...> : public type_subsequence<Result, N-1, Args...>
		{
			static_assert(N < sizeof...(Args) + 1);
		};
		
		template <template <class...> class Result, class... Args>
		struct type_subsequence<Result, 0, Args...>
		{
			typedef Result<Args...> result;
			typedef tuple<Args...> sequence;
		};
		
		template <template <class...> class Result, class T, class... Args>
		struct type_subsequence<Result, 0, T, Args...>
		{
			typedef Result<T, Args...> result;
			typedef tuple<T, Args...> sequence;
		};
		
	}
	
	
	template <class T, class... Y>
	class tuple<T, Y...> : protected tuple<Y...>
	{
		protected:
		typedef tuple<Y...> base_type;
		T item;
		//detail::tuple_base<Y...> base;
		public:
		
		constexpr tuple()
		{
			
		}
		
		explicit constexpr tuple(const T& t, const Y&... vals) : item(t), base_type(vals...)
		{
			
		}
		
		template <class Arg, class... Args>
		explicit constexpr tuple(Arg&& a, Args&&... args) : item(forward<Arg&&>(a)), base_type(forward<Args>(args)...)
		{
			
		}
		
		template <class Arg1, class... Args>
		constexpr tuple(const tuple<Arg1, Args...>& other);
		
		template <class Arg1, class... Args>
		constexpr tuple(tuple<Arg1, Args...>&& other);
		
		tuple(const tuple&) = default;
		tuple(tuple&&) = default;
		
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
			typedef typename detail::type_subsequence<tuple, N, T, Types...>::result base_type;
			return ((base_type&)tup).item;
			//return detail::tuple_get_helper<N, T, Types...>::call(tup.base);
		}
		
		constexpr static const type& get(const tuple<T, Types...>& tup)
		{
			typedef typename detail::type_subsequence<tuple, N, T, Types...>::result base_type;
			return ((const base_type&)tup).item;
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