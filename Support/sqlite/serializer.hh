#ifndef INCLUDED_SUPPORT_SQLITE_SERIALIZER_HH
#define INCLUDED_SUPPORT_SQLITE_SERIALIZER_HH

#include <Common.h>
#include "sqlite_type.hh"
#include <Utils/utility>
#include <Utils/constexpr_string.hh>
#include <Utils/tuple>
#include <Support/MetaProgramming/Utilities.hh>

#if __cplusplus >= 201703L

namespace Support { namespace SQLite
{
	namespace detail
	{
		template <class>
		struct member_pointer_extractor;
		
		template <class Y, class T>
		struct member_pointer_extractor<Y T::*>
		{
			typedef T object_type;
			typedef Y member_type;
		};
		
		
		
		
		
		
		template <class... Args>
		struct invoke_all
		{
			typedef void(*fn)(Args...);
			
			
			template <fn func1, fn func2, fn... functions>
			__attribute__((__always_inline__))
			constexpr static void invoke(Args... args)
			{
				func1(args...);
				invoke<func2, functions...>(args...);
			}
			
			template <fn func>
			__attribute__((__always_inline__))
			constexpr static void invoke(Args... args)
			{
				func(args...);
			}
			
			template <fn f1, fn... funcs>
			struct inner
			{
				constexpr static void invoke(Args... args)
				{
					invoke_all<Args...>::template invoke<f1, funcs...>(args...);
				}
			};
		};
		
		
		
		
		
		template <unsigned...>
		struct n_sequence { };
		
		template <long N, long... Ns>
		struct sequence_gen : public sequence_gen<N-1, N, Ns...>
		{
			static_assert(N > 0);
		};
		
		template <long... Ns>
		struct sequence_gen<0, Ns...>
		{
			typedef n_sequence<0, Ns...> type;
		};
		
		template <unsigned N, unsigned Max>
		struct _invoke_for_each_tuple
		{
			constexpr static void call(auto& tup, auto fn, auto... prefix_args)
			{
				fn(prefix_args..., Utils::get<N>(tup));
				_invoke_for_each_tuple<N+1, Max>::call(tup, fn, prefix_args...);
			}
		};
		
		template <unsigned Max>
		struct _invoke_for_each_tuple<Max, Max>
		{
			constexpr static void call(auto& tup, auto fn, auto... prefix_args)
			{
				fn(prefix_args..., Utils::get<Max>(tup));
			}
		};
		
		template <class Tup>
		struct invoke_for_each_tuple;
		
		template <class... Args>
		struct invoke_for_each_tuple<Utils::tuple<Args...>>
		{
			
			constexpr static void call(auto& tup, auto fn, auto... prefix_args)
			{
				_invoke_for_each_tuple<0, sizeof...(Args)-1>::call(tup, fn, prefix_args...);
			}
		};
		
		template <>
		struct invoke_for_each_tuple<Utils::tuple<>>
		{
			
			constexpr static void call(auto& tup, auto fn, auto... prefix_args)
			{
				
			}
		};
		
		
		
		
		template <class T>
		struct type_references
		{
			typedef typename Utils::decay<T>::type type;
			typedef type* pointer;
			typedef const type* const_pointer;
		};
		
		
		
	}
	
	
	template <class... B>
	struct base_types
	{
		template <class T>
		struct verify_inheritance : public Utils::integral_constant<bool, (Utils::is_base_of<B, T>::value && ...)>
		{
			static_assert((Utils::is_base_of<B, T>::value && ...));
		};
	};
	
	
	
	
	template <class Y, class T>
	struct Member_Pointer
	{
		typedef Y T::* type;
	};
	
	template <class T>
	struct model
	{
		constexpr static bool defined = false;
		
	};
	
	template <class T, class = void>
	struct has_model : public Utils::integral_constant<bool, true>
	{
		
	};
	
	template <class T>
	struct has_model<T, Utils::void_t<decltype(model<T>::defined)>> : public Utils::integral_constant<bool, model<T>::defined>
	{
		
	};
	
	template <class T, class = void>
	struct is_primitive : public Utils::integral_constant<bool, false>
	{
		
	};
	
	template <class T>
	struct is_primitive<T, Utils::void_t<decltype(model<T>::type_enum)>> : public Utils::integral_constant<bool, model<T>::type_enum != 0>
	{
		constexpr static sqlite_type type_enum = model<T>::type_enum;
	};
	
	template <class T>
	struct is_entity : public Utils::integral_constant<bool, has_model<T>::value && !is_primitive<T>::value>
	{
		
	};
	
	
	template <class T, class = void>
	struct model_has_members : public Utils::integral_constant<bool, false>
	{};
	
	template <class T>
	struct model_has_members<T, Utils::void_t<decltype(model<T>::members)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto members = model<T>::members;
	};
	
	template <class T>
	struct model_has_members<T, Utils::void_t<decltype(model<T>::Members)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto members = model<T>::Members;
	};
	
	
	template <class T, class = void>
	struct model_has_name : public Utils::integral_constant<bool, false>
	{};
	
	template <class T>
	struct model_has_name<T, Utils::void_t<decltype(model<T>::name)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto name = model<T>::name;
	};
	
	template <class T>
	struct model_has_name<T, Utils::void_t<decltype(model<T>::Name)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto name = model<T>::Name;
	};
	
	
	template <class T, class = void>
	struct model_has_bases : public Utils::integral_constant<bool, false>
	{
		typedef base_types<> bases;
	};
	
	template <class T>
	struct model_has_bases<T, Utils::void_t<decltype(model<T>::Bases)>> : public Utils::integral_constant<bool, true>
	{
		typedef typename Utils::decay<decltype(model<T>::Bases)>::type bases;
		
		private:
		typedef typename bases::template verify_inheritance<T> verify_t;
		constexpr static verify_t verification = verify_t();
	};
	
	template <class T>
	struct model_has_bases<T, Utils::void_t<decltype(model<T>::bases)>> : public Utils::integral_constant<bool, true>
	{
		typedef typename Utils::decay<decltype(model<T>::bases)>::type bases;
		
		private:
		typedef typename bases::template verify_inheritance<T> verify_t;
		constexpr static verify_t verification = verify_t();
	};
	
	template <class T>
	struct model_has_bases<T, Utils::void_t<decltype(model<T>::Base)>> : public Utils::integral_constant<bool, true>
	{
		typedef typename Utils::decay<decltype(model<T>::Base)>::type bases;
		
		private:
		typedef typename bases::template verify_inheritance<T> verify_t;
		constexpr static verify_t verification = verify_t();
	};
	
	template <class T>
	struct model_has_bases<T, Utils::void_t<decltype(model<T>::base)>> : public Utils::integral_constant<bool, true>
	{
		typedef typename Utils::decay<decltype(model<T>::base)>::type bases;
		
		private:
		typedef typename bases::template verify_inheritance<T> verify_t;
		constexpr static verify_t verification = verify_t();
	};
	
	
	
	
	template <class T, class = void>
	struct model_has_key;
	
	template <class T>
	struct model_has_key<T, Utils::void_t<decltype(model<T>::key)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto key = model<T>::key;
		typedef typename decltype(key)::member_type key_type;
		
		
		__attribute__((__always_inline__))
		static const key_type* get_key(const T* obj)
		{
			return &(obj->*key.member);
		}
		
		__attribute__((__always_inline__))
		static key_type* get_key(T* obj)
		{
			return &(obj->*key.member);
		}
	};
	
	template <class T>
	struct model_has_key<T, Utils::void_t<decltype(model<T>::Key)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto key = model<T>::Key;
		typedef typename decltype(key)::member_type key_type;
		
		__attribute__((__always_inline__))
		static const key_type* get_key(const T* obj)
		{
			return &(obj->*key.member);
		}
		
		__attribute__((__always_inline__))
		static key_type* get_key(T* obj)
		{
			return &(obj->*key.member);
		}
	};
	
	template <class T>
	struct model_has_key<T, Utils::void_t<decltype(model<T>::keys)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto key = model<T>::keys;
		typedef typename decltype(key)::member_type key_type;
		
		__attribute__((__always_inline__))
		static const key_type* get_key(const T* obj)
		{
			return &(obj->*key.member);
		}
		
		__attribute__((__always_inline__))
		static key_type* get_key(T* obj)
		{
			return &(obj->*key.member);
		}
	};
	
	template <class T>
	struct model_has_key<T, Utils::void_t<decltype(model<T>::Keys)>> : public Utils::integral_constant<bool, true>
	{
		constexpr static auto key = model<T>::Keys;
		typedef typename decltype(key)::member_type key_type;
		
		__attribute__((__always_inline__))
		static const key_type* get_key(const T* obj)
		{
			return &(obj->*key.member);
		}
		
		__attribute__((__always_inline__))
		static key_type* get_key(T* obj)
		{
			return &(obj->*key.member);
		}
		
	};
	
	namespace detail
	{
		
		template <class T, bool Value, class... Ts>
		struct types_have_key;
		
		template <class T>
		struct types_have_key<T, false>  : public Utils::integral_constant<bool, false>
		{
			typedef void key_type;
			__attribute__((__always_inline__))
			static const key_type* get_key(const T* obj)
			{
				return nullptr;
			}
			__attribute__((__always_inline__))
			static key_type* get_key(T* obj)
			{
				return nullptr;
			}
		};
		
		template <class T, class T2, class... Ts>
		struct types_have_key<T, false, T2, Ts...> : public types_have_key<T2, model_has_key<T2>::value, Ts...>
		{
			
		};
		
		template <class T, class... Ts>
		struct types_have_key<T, true, Ts...> : public model_has_key<T>
		{
			
		};
		
		template <class T, class... Ts>
		struct types_have_key_fwd : public types_have_key<T, model_has_key<T>::value, Ts...>
		{
			
		};
		
		
		template <class T, bool HasBases>
		struct model_has_bases_key;
		
		template <class T>
		struct model_has_bases_key<T, false> : public Utils::integral_constant<bool, false>
		{
			typedef void key_type;
			__attribute__((__always_inline__))
			static const key_type* get_key(const T* obj)
			{
				return nullptr;
			}
			__attribute__((__always_inline__))
			static key_type* get_key(T* obj)
			{
				return nullptr;
			}
		};
		
		template <class>
		struct bases_has_key;
		
		template <template <class...> class T, class... B>
		struct bases_has_key<T<B...>> : public types_have_key_fwd<B...>
		//public Utils::integral_constant<bool, (model_has_key<B>::value && ...)>
		// types_have_key<B...>::value>
		{
			
		};
		
		template <template <class...> class T, class... B>
		struct bases_has_key<const T<B...>> : public bases_has_key<T<B...>>
		{
			
		};
		
		template <class T>
		struct model_has_bases_key<T, true> : public bases_has_key<typename model_has_bases<T>::bases>
		{
			
		};
	}
	
	template <class T, class>
	struct model_has_key : public detail::model_has_bases_key<T, model_has_bases<T>::value>
	{
		
	};
	
	
	
	
	
	
	
	
	
	template <auto ptr>
	class Member_Ptr
	{
		typedef detail::member_pointer_extractor<decltype(ptr)> extractor_type;
		public:
		typedef typename extractor_type::object_type T;
		typedef typename extractor_type::member_type Y;
		
		template <unsigned N>
		struct Member
		{
			typedef Member_Ptr::T object_type;
			typedef Member_Ptr::Y member_type;
			constexpr static bool is_member = true;
			typedef model<typename Utils::decay<member_type>::type> member_model;
			
			
			const char (&name) [N];
			constexpr static auto member = ptr;
			
			constexpr Member(const char (&_name) [N]) : name(_name)
			{
				static_assert(ptr != nullptr);
				static_assert(N > 1);
			}
			
			constexpr Member(const Member& m) : name(m.name)
			{
				static_assert(ptr != nullptr);
				static_assert(N > 1);
			}
			
			__attribute__((__always_inline__))
			constexpr void store(const T* obj, auto& storage) const
			{
				storage[name] = obj->*ptr;
			}
			
			template <class Context>
			using store_fn = void(*)(const T*, Context&);
			
			template <class Context>
			constexpr store_fn<Context> get_store() const
			{
				return [](const T* t, Context& c) -> void
				{
					c[""] = t->*ptr;
				};
			}
			
			template <class Context>
			constexpr static store_fn<Context> get_store2(Member m)
			{
				constexpr auto n = m.name;
			}
		};
		
		
	};
	
	
	
	namespace detail
	{
		template <template <class> class Checker, class... Types>
		struct type_sequence_checker;
		
		template <template <class> class Checker, class T1, class... Types>
		struct type_sequence_checker<Checker, T1, Types...> : public Utils::integral_constant<bool,  type_sequence_checker<Checker, Types...>::value && Checker<T1>::value>
		{
			
		};
		
		template <template <class> class Checker>
		struct type_sequence_checker<Checker> : public Utils::integral_constant<bool, true>
		{
			
		};
	}
	
	template <class, class = void>
	struct is_member_type : public Utils::integral_constant<bool, false>
	{};
	
	template <class T>
	struct is_member_type<T, Utils::void_t<decltype(T::is_member)>> : public Utils::integral_constant<bool, T::is_member>
	{};
	
	template <class Y, class T>
	struct is_member_of : public Utils::integral_constant<bool, Utils::is_same<T, typename Y::object_type>::value>
	{};
	
	
	template <class T, class... MTypes>
	class Members
	{
		private:
		template <class Y>
		struct get_member_ptr_type
		{
			typedef typename Y::member_type type;
		};
		
		
		public:
		
		typedef typename detail::type_references<T>::pointer pointer;
		typedef typename detail::type_references<T>::const_pointer const_pointer;
		
		
		
		private:
		
		
		template <class J>
		using MPtr = typename Member_Pointer<J, T>::type;
		
		template <class J>
		using object_t_checker = is_member_of<J, T>;
		static_assert(detail::type_sequence_checker<is_member_type, MTypes...>::value);
		static_assert(detail::type_sequence_checker<object_t_checker, MTypes...>::value);
		
		
		
		
		public:
		typedef T object_type;
		const Utils::tuple<MTypes...> members;
		constexpr static size_t size = sizeof...(MTypes);
		typedef typename Meta::type_sequence<MTypes...>::template apply_transforms<get_member_ptr_type> member_types;
		typedef typename member_types::template apply_transforms<Utils::remove_pointer>::template where<is_entity> member_entity_types;
		
		constexpr Members(auto mems) : members(Utils::forward<decltype(mems)>(mems))
		{
			
		}
		
		constexpr Members(const Utils::tuple<MTypes...>& mems) : members(mems)
		{
			
		}
		
		
		
		template <class Context>
		__attribute__((__always_inline__))
		constexpr bool store(const_pointer obj, Context& context) const
		{
			//constexpr Utils::tuple<MTypes...> mems(this->members);
			//constexpr auto mems = members;
			
			typedef void(*Fn)(const_pointer, Context&);
			//constexpr Fn f = m1.get_store();
			
			assert(NOT_IMPLEMENTED);
		}
		
		
		constexpr void eval_for_each(auto func, auto... args) const
		{
			//func(args..., Utils::get<0>(members));
			detail::invoke_for_each_tuple<Utils::tuple<MTypes...>>::call(members, func, args...);
		}
		
		private:
	};
	
	template <class T>
	struct __value_holder
	{
		template <T...>
		struct values
		{};
	};
	
	template <class T, T... v>
	using value_holder = typename __value_holder<T>::template values<v...>;
	
	
	
	template <class Context, class M, class>
	struct store_functions;
	
	template <class Context, class M>
	struct __storing
	{
		typedef typename decltype(M::members)::object_type object_type;
		typedef void(*Fn)(const object_type*, Context&);
		
		template <Fn... funcs>
		using Fn_Collection = typename __value_holder<Fn>::template values<funcs...>;
		
		template <unsigned N>
		static void store_n(const object_type* obj, Context& c)
		{
			constexpr static auto member = Utils::get<N>(M::members.members);
			constexpr static auto name = member.name;
			constexpr static auto fn_ptr = member.member;
			c[name] = obj->*fn_ptr;
			
		}
		
		template <unsigned N>
		constexpr static Fn get_n()
		{
			constexpr auto members = M::members.members;
			constexpr auto member = Utils::get<N>(members);
			constexpr auto name = member.name;
			return [](const object_type* t, Context& c) -> void
			{
				constexpr auto fn_ptr = member.member;
				c[name] = t->*fn_ptr;
			};
		}
		
		private:
		
		template <unsigned Count>
		using nsequence = typename detail::sequence_gen<Count-1>::type;
		
		public:
		
		template <unsigned N>
		constexpr static Fn fn_at = get_n<N>();
		
		template <unsigned N>
		constexpr static void store(const object_type* o, Context& c)
		{
			constexpr Fn f = get_n<N>();
			f(o, c);
		}
	};
	
	template <class Context, class M, unsigned... Ns>
	struct store_functions<Context, M, detail::n_sequence<Ns...>>
	{
		typedef typename __storing<Context, M>::Fn Fn;
		
		template <Fn... funcs>
		using Fn_Collection = typename __value_holder<Fn>::template values<funcs...>;
		
		
		
		typedef Fn_Collection<__storing<Context, M>::template store<Ns>...> type;
		
	};
	
	template <class Context, class M>
	struct storing
	{
		
		typedef typename store_functions<Context, M, typename detail::sequence_gen<decltype(M::members)::size-1>::type>::type Functions;
	};
	
	
	template <class Context, class M, unsigned Count, unsigned N = 0>
	struct __store_fn_merger
	{
		typedef typename decltype(M::members)::object_type object_type;
		
		__attribute__((__always_inline__))
		static void fn(const object_type* obj, Context& c)
		{
			constexpr auto member = Utils::get<N>(M::members.members);
			constexpr auto name = member.name;
			constexpr auto mem_ptr = member.member;
			
			typedef typename Utils::remove_pointer<typename decltype(member)::member_type>::type field_type;
			
			if constexpr (is_entity<field_type>::value)
			{
				static_assert(!is_entity<field_type>::value || model_has_key<field_type>::value);
				if constexpr (!Utils::is_pointer<typename decltype(member)::member_type>::value)
				{
					c.store(name, *model_has_key<field_type>::get_key(obj->*mem_ptr));
					
				}
				else if (obj->*mem_ptr != nullptr)
				{
					auto key = model_has_key<field_type>::get_key(obj->*mem_ptr);
					assert(key);
					c.store(name, *key);
					c.store(obj->*mem_ptr);
				}
				else
				{
					c.store(name, nullptr);
				}
			}
			else
			{
				c.store(name, obj->*mem_ptr);
			}
			__store_fn_merger<Context, M, Count, N+1>::fn(obj, c);
		}
	};
	
	template <class Context, class M, unsigned Count>
	struct __store_fn_merger<Context, M, Count, Count>
	{
		typedef typename decltype(M::members)::object_type object_type;
		
		__attribute__((__always_inline__))
		static void fn(const object_type* obj, Context& c)
		{
			typedef typename M::type type;
			if constexpr (model_has_key<type>::value)
			{
				constexpr auto key = model_has_key<type>::key;
				constexpr auto ptr = key.member;
				c.store(key.name, obj->*ptr);
			}
		}
	};
	
	template <class Context, class T, class>
	struct base_store_caller;
	
	template <class Context, class T, class B, class... Bs>
	struct base_store_caller<Context, T, base_types<B, Bs...>>
	{
		__attribute__((__always_inline__))
		constexpr static auto Store(const T* obj, Context& c)
		{
			c.store((B*)obj);
			return base_store_caller<Context, T, base_types<Bs...>>::Store(obj, c);
		}
	};
	
	template <class Context, class T, class B>
	struct base_store_caller<Context, T, base_types<B>>
	{
		__attribute__((__always_inline__))
		constexpr static auto Store(const T* obj, Context& c)
		{
			return c.store((B*)obj);
		}
	};
	
	
	template <class Context, class M>
	struct store_fn_merger
	{
		typedef typename decltype(M::members)::object_type object_type;
		
		typedef typename __storing<Context, M>::Fn Fn;
		
		//constexpr static Fn Store = __store_fn_merger<Context, M, decltype(M::members)::size>::fn;
		
		__attribute__((__always_inline__))
		constexpr static void Store(const object_type* obj, Context& c)
		{
			if constexpr (model_has_bases<object_type>::value)
			{
				base_store_caller<Context, object_type, typename model_has_bases<object_type>::bases>::Store(obj, c);
			}
			__store_fn_merger<Context, M, decltype(M::members)::size>::fn(obj, c);
		}
	};
	
	
	
	
	
	
	
	
	
	
	
	
	template <class Context, class M, unsigned Count, unsigned N = 0>
	struct __load_fn_merger
	{
		typedef typename decltype(M::members)::object_type object_type;
		
		__attribute__((__always_inline__))
		static void fn(object_type* obj, Context& c)
		{
			constexpr auto member = Utils::get<N>(M::members.members);
			constexpr auto name = member.name;
			constexpr auto mem_ptr = member.member;
			
			typedef typename Utils::remove_pointer<typename decltype(member)::member_type>::type field_type;
			
			if constexpr (is_entity<field_type>::value)
			{
				static_assert(!is_entity<field_type>::value || model_has_key<field_type>::value);
				if constexpr (!Utils::is_pointer<typename decltype(member)::member_type>::value)
				{
					
					obj->*mem_ptr = *c.template load<field_type>(c.template get_value<typename model_has_key<field_type>::key_type>(name));
					//c.load<(name, *model_has_key<field_type>::get_key(obj->*mem_ptr));
				}
				else if (obj->*mem_ptr != nullptr)
				{
					obj->*mem_ptr = c.template load<field_type>(c.template get_value<typename model_has_key<field_type>::key_type>(name));
					//c.store(name, *model_has_key<field_type>::get_key(obj->*mem_ptr));
				}
			}
			else
			{
				c.load(name, obj->*mem_ptr);
				//c.store(name, obj->*mem_ptr);
			}
			__load_fn_merger<Context, M, Count, N+1>::fn(obj, c);
		}
	};
	
	template <class Context, class M, unsigned Count>
	struct __load_fn_merger<Context, M, Count, Count>
	{
		typedef typename decltype(M::members)::object_type object_type;
		
		__attribute__((__always_inline__))
		static void fn(object_type* obj, Context& c)
		{
			typedef typename M::type type;
			if constexpr (model_has_key<type>::value)
			{
				constexpr auto key = model_has_key<type>::key;
				constexpr auto ptr = key.member;
				c.load(key.name, obj->*ptr);
			}
		}
	};
	
	
	template <class Context, class T, class>
	struct base_load_caller;
	
	template <class Context, class T, class B>
	struct base_load_caller<Context, T, base_types<B>>
	{
		__attribute__((__always_inline__))
		constexpr static auto Load(T* obj, Context& c)
		{
			return c.load((B*)obj);
		}
	};
	
	
	
	template <class Context, class M>
	struct load_fn_merger
	{
		typedef typename decltype(M::members)::object_type object_type;
		
		typedef void(*Fn)(object_type*, Context&);
		
		__attribute__((__always_inline__))
		constexpr static void Load(object_type* obj, Context& c)
		{
			if constexpr (model_has_bases<object_type>::value)
			{
				base_load_caller<Context, object_type, typename model_has_bases<object_type>::bases>::Load(obj, c);
			}
			__load_fn_merger<Context, M, decltype(M::members)::size>::fn(obj, c);
		}
	};
	
	template <unsigned N, class Context, class M>
	constexpr bool __store()
	{
		constexpr auto mems = M::members.members;
		
		constexpr auto m = Utils::get<N>(mems);
		constexpr auto j = m.template get_store<Context>();
		
		constexpr auto n = m.name;
		
		typedef void(*Fn)(const typename decltype(M::members)::object_type*, Context&);
		
		constexpr Fn f = [](auto t, Context& c) -> void
		{
			constexpr auto mem = m.member;
			c[n] = t->*mem;
		};
		
		//decltype(m)::template get_store2<Context>(m);
		
		constexpr Fn f2 = __storing<Context, M>::template fn_at<N>;
		static_assert(f2);
		
		typedef value_holder<Fn, __storing<Context, M>::template store<N>> gyrt;
		typedef typename storing<Context, M>::Functions srcerd;
		assert(NOT_IMPLEMENTED);
	}
	
	namespace detail
	{
		template <template <class...> class Result, class T, class... PrefixArgs>
		struct template_arg_extractor;
		
		template <template <class...> class Result, template <class...> class T, class... Args, class... PrefixArgs>
		struct template_arg_extractor<Result, T<Args...>, PrefixArgs...>
		{
			typedef Result<PrefixArgs..., Args...> type;
		};
	}
	
	template <class T>
	constexpr auto get_members(auto mems)
	{
		
		typedef typename detail::template_arg_extractor<Members, typename Utils::decay<decltype(mems)>::type, T>::type type;
		return type(mems);
	}
	
	template <class... Types>
	struct key_types
	{
		
	};
	
	
	namespace detail
	{
		
		template <class T, template <T...> class Result, class Input>
		struct value_applier;
		
		template <class T, template <T...> class Result, template <T...> class Input, T... vals>
		struct value_applier<T, Result, Input<vals...>>
		{
			typedef Result<vals...> type;
		};
		
		
		template <class T>
		struct key_type_getter;
		
		template <class T>
		struct all_bases_finder
		{
			private:
			typedef typename Meta::type_sequence_from<typename model_has_bases<T>::bases>::type direct;
			typedef typename direct::template select_many<all_bases_finder>::distinct next_direct;
			
			
			public:
			typedef typename direct::template concat<next_direct>::distinct type;
			
		};
		
		
		
		template <class T>
		class BuiltModel
		{
			public:
			typedef typename type_references<T>::type type;
			typedef typename type_references<T>::pointer pointer;
			typedef typename type_references<T>::const_pointer const_pointer;
			//static_assert(
			
			
			static_assert(model_has_members<T>::value);
			static_assert(model_has_name<T>::value);
			
			constexpr static auto members = get_members<T>(model_has_members<T>::members);
			typedef typename decltype(members)::member_types member_types;
			typedef typename decltype(members)::member_entity_types member_entity_types;
			static_assert(model_has_name<T>::value, "Model needs a name");
			constexpr static auto name = model_has_name<T>::name;
			
			
			constexpr static bool has_key = model_has_key<T>::value;
			typedef typename model_has_key<T>::key_type key_type;
			//constexpr static key_type(T::*key_ptr) = model_has_key<T>::key.member;
			
			typedef typename model_has_bases<T>::bases Bases;
			typedef typename all_bases_finder<T>::type All_Bases;
			
			__attribute__((__always_inline__))
			static key_type* get_key(type* obj)
			{
				return model_has_key<T>::get_key(obj);
			}
			
			__attribute__((__always_inline__))
			static const key_type* get_key(const type* obj)
			{
				return model_has_key<T>::get_key(obj);
			}
			
			__attribute__((__always_inline__))
			static void set_key(type* obj, const key_type& k)
			{
				return model_has_key<T>::set_key(obj, k);
			}
			
			
			/*template <class Context>
			__attribute__((__always_inline__))
			static void Store(const_pointer obj, Context& context)
			{
				__store<0, Context, BuiltModel>();
				members.store(obj, context);
			}*/
			
			template <class Context, class Return = void>
			using store_fn = Return(*)(const_pointer, Context&);
			
			template <class Context, class Return = void>
			using load_fn = Return(*)(pointer, Context&);
			
			private:
			
			/*template <class Context>
			using funcs = typename storing<Context, BuiltModel>::Functions;
			
			template <class Context>
			using invoker = typename invoke_all<const_pointer, Context&>::inner;*/
			
			/*template <class Context, class Funcs, template <store_fn<Context>...> class Result>
			using vapp = typename value_applier<store_fn<Context>, Result, Funcs>::type;*/
			
			/*template <class Context, class Funcs>
			using vapp = typename value_applier<store_fn<Context>, invoke_all<const_pointer, Context&>::template inner, Funcs>::type;*/
			
			public:
			
			
			
			/*template <class Context>
			constexpr static store_fn<Context> store = vapp<Context, funcs<Context>>::invoke;*/
			
			template <class Context>
			constexpr static store_fn<Context> store = store_fn_merger<Context, BuiltModel>::Store;
			
			template <class Context>
			constexpr static load_fn<Context> load = load_fn_merger<Context, BuiltModel>::Load;
			
		};
		
		
		
		template <class T>
		struct model_member_entity_types
		{
			typedef typename BuiltModel<T>::member_entity_types type;
		};
		
		
		template <class T, class Found = Meta::type_sequence<>, bool = true>
		struct all_related_entity_types;
		
		template <class T, class Found>
		struct all_related_entity_types<T, Found, false>
		{
			typedef typename Found::distinct::template remove<T>::template concat<Meta::type_sequence<T>>::template where<is_entity> type;
		};
		
		template <class T, class Found>
		struct all_related_entity_types<T, Found, true>
		{
			private:
			typedef typename all_related_entity_types<T, Found, false>::type Distinct;
			typedef typename Distinct::template select_many<model_member_entity_types>::distinct::template concat<Distinct> S1;
			constexpr static auto are_same = S1::template same_types<Found>;
			
			public:
			typedef typename all_related_entity_types<T, S1, !are_same>::type type;
			
		};
	}
	
	
	
	
	
	namespace detail
	{
		template <class T>
		struct model_type_extractor;
		
		template <class T>
		struct model_type_extractor<model<T>>
		{
			typedef T type;
		};
	}
	
	#ifdef MEMBER
	#error MEMBER already defined
	#endif
	
	#define MEMBER(X) ::Support::SQLite::Member_Ptr<&::Support::SQLite::detail::model_type_extractor<model>::type:: X>::Member(#X)
	
	
	template <>
	struct model<int>
	{
		constexpr static bool defined = true;
		constexpr static sqlite_type type_enum = Integer;
		
	};
	
	template <>
	struct model<long>
	{
		constexpr static bool defined = true;
		constexpr static sqlite_type type_enum = Integer;
		
	};
	
	template <>
	struct model<double>
	{
		constexpr static bool defined = true;
		constexpr static sqlite_type type_enum = Float;
		
	};
	
	
	
	
	
	namespace detail
	{
		template <class T, bool primitive, bool entity, bool pointer, bool hasModel>
		struct enum_type;
		
		template <class T, bool primitive, bool entity, bool hasModel>
		struct enum_type<T, primitive, entity, true, hasModel> : public enum_type<typename Utils::remove_pointer<T>::type, is_primitive<typename Utils::remove_pointer<T>::type>::value, is_entity<typename Utils::remove_pointer<T>::type>::value, Utils::is_pointer<typename Utils::remove_pointer<T>::type>::value, has_model<typename Utils::remove_pointer<T>::type>::value>
		{};
		
		template <class T>
		struct enum_type<T, true, false, false, true>
		{
			constexpr static sqlite_type value = model<T>::type_enum;
		};
		
		template <class T>
		struct enum_type<T, false, true, false, true>
		{
			private:
			typedef typename BuiltModel<T>::key_type type;
			
			public:
			
			constexpr static sqlite_type value = enum_type<type, is_primitive<type>::value, is_entity<type>::value, Utils::is_pointer<type>::value, has_model<type>::value>::value;
		};
	}
	
	
	template <class T>
	struct enum_type
	{
		constexpr static sqlite_type value = detail::enum_type<T, is_primitive<T>::value, is_entity<T>::value, Utils::is_pointer<T>::value, has_model<T>::value>::value;
	};
	
	template <sqlite_type>
	struct type_from_enum;
	
	template <>
	struct type_from_enum<Integer>
	{
		typedef long long type;
		typedef const long long pass_type;
	};
	
	template <>
	struct type_from_enum<Float>
	{
		typedef long double type;
		typedef const long double pass_type;
	};
	
	template <>
	struct type_from_enum<Text>
	{
		typedef Utils::string type;
		typedef const Utils::string& pass_type;
	};
	
	
	
	
	
}
}

#endif
#endif