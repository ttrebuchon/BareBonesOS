#ifndef INCLUDED_SUPPORT_METAPROGRAMMING_UTILITIES_HH
#define INCLUDED_SUPPORT_METAPROGRAMMING_UTILITIES_HH

#include <Common.h>
#include <Utils/utility>
#include <Utils/typeindex>

namespace Support { namespace Meta
{
	
	
	template <class... Types>
	struct type_sequence;
	
	template <class T, T... values>
	struct number_sequence;
	
	
	
	
	
	
	
	// indexof_type<T, T<T...>>::value
	template <class T, class Sequence, size_t = 0>
	struct indexof_type;
	
	template <size_t N, class T, template <class...> class Sequence, class T1, class... Types>
	struct indexof_type<T, Sequence<T1, Types...>, N> : public indexof_type<T, Sequence<Types...>, N+1>
	{
		
	};
	
	template <size_t N, class T, template <class...> class Sequence, class... Types>
	struct indexof_type<T, Sequence<T, Types...>, N> : public Utils::integral_constant<size_t, N>
	{
		constexpr static bool found = true;
		constexpr static size_t npos = (size_t(0)-1);
	};
	
	template <size_t N, class T, template <class...> class Sequence>
	struct indexof_type<T, Sequence<>, N> : public Utils::integral_constant<size_t, (size_t(0)-1)>
	{
		constexpr static bool found = false;
		constexpr static size_t npos = (size_t(0)-1);
	};
	
	
	
	// type_sequence_append<S<T...>, T>::type
	template <class S, class T>
	struct type_sequence_append;
	
	template <template <class...> class Seq, class... Types, class T>
	struct type_sequence_append<Seq<Types...>, T>
	{
		typedef Seq<Types..., T> type;
	};
	
	
	// type_sequence_contains<T, T<T...>>::value
	
	template <class T, class>
	struct type_sequence_contains;
	
	template <class T, template <class...> class Sequence, class T1, class... Types>
	struct type_sequence_contains<T, Sequence<T1, Types...>> : public type_sequence_contains<T, Sequence<Types...>>
	{};
	
	template <class T, template <class...> class Sequence, class... Types>
	struct type_sequence_contains<T, Sequence<T, Types...>> : public Utils::integral_constant<bool, true>
	{};
	
	template <class T, template <class...> class Sequence>
	struct type_sequence_contains<T, Sequence<>> : public Utils::integral_constant<bool, false>
	{};
	
	
	// type_sequence_filter<T, T<T...>>::value
	
	template <template <class...> class T, class, class = void, bool = false>
	struct type_sequence_filter;
	
	template <template <class...> class T, template <class...> class Sequence, class T1, class... Types, bool B>
	struct type_sequence_filter<T, Sequence<T1, Types...>, void, B> : public type_sequence_filter<T, Sequence<T1, Types...>, Sequence<>, T<T1>::value>
	{};
	
	template <template <class...> class T, template <class...> class Sequence, class T1, class T2, class... Types, class... Filtered>
	struct type_sequence_filter<T, Sequence<T1, T2, Types...>, Sequence<Filtered...>, true> : public type_sequence_filter<T, Sequence<T2, Types...>, Sequence<Filtered..., T1>, T<T2>::value>
	{};
	
	template <template <class...> class T, template <class...> class Sequence, class T1, class T2, class... Types, class... Filtered>
	struct type_sequence_filter<T, Sequence<T1, T2, Types...>, Sequence<Filtered...>, false> : public type_sequence_filter<T, Sequence<T2, Types...>, Sequence<Filtered...>, T<T2>::value>
	{};
	
	template <template <class...> class T, template <class...> class Sequence, class T1, class... Filtered>
	struct type_sequence_filter<T, Sequence<T1>, Sequence<Filtered...>, true>
	{
		typedef Sequence<Filtered..., T1> type;
	};
	
	template <template <class...> class T, template <class...> class Sequence, class T1, class... Filtered>
	struct type_sequence_filter<T, Sequence<T1>, Sequence<Filtered...>, false>
	{
		typedef Sequence<Filtered...> type;
	};
	
	
	
	// type_sequence_from<T...>::type
	template <class>
	struct type_sequence_from;
	
	template <template <class...> class Seq, class... Types>
	struct type_sequence_from<Seq<Types...>>
	{
		typedef type_sequence<Types...> type;
	};
	
	
	
	// type_sequence_concat<T<T...>, T<T...>>::type
	template <class, class...>
	struct type_sequence_concat;
	
	/*template <template <class...> class Seq, class... Types, class... T>
	struct type_sequence_concat<Seq<Types...>, T...>
	{
		typedef Seq<Types..., T...> type;
	};*/
	
	template <template <class...> class Seq, class... Types, template <class...> class Seq2, class... Types2>
	struct type_sequence_concat<Seq<Types...>, Seq2<Types2...>>
	{
		typedef Seq<Types..., Types2...> type;
	};
	
	
	
	// type_sequence_select_many<T, T<T...>>::type
	template <template <class...> class T, class>
	struct type_sequence_select_many;
	
	template <template <class...> class T, template <class...> class Seq, class Type1, class... Types>
	struct type_sequence_select_many<T, Seq<Type1, Types...>> : public type_sequence_concat<typename type_sequence_concat<Seq<>, typename T<Type1>::type>::type, typename type_sequence_select_many<T, Seq<Types...>>::type>
	{
		
	};
	
	template <template <class...> class T, template <class...> class Seq, class Type>
	struct type_sequence_select_many<T, Seq<Type>> : public type_sequence_concat<Seq<>, typename T<Type>::type>
	{
		
	};
	
	template <template <class...> class T, template <class...> class Seq>
	struct type_sequence_select_many<T, Seq<>>
	{
		typedef Seq<> type;
	};
	
	
	
	// type_sequence_distinct<T<T...>>::type
	template <class>
	struct type_sequence_distinct;
	
	namespace detail { namespace distinct
	{
		template <class, class, bool = false>
		struct ts_distinct;
		
		template <template <class...> class Seq, class... T1, class T2_1, class T2_2, class... T2>
		struct ts_distinct<Seq<T1...>, Seq<T2_1, T2_2, T2...>, false>
		{
			private:
			typedef Seq<T1..., T2_1> S1;
			
			public:
			typedef typename ts_distinct<S1, Seq<T2_2, T2...>, type_sequence_contains<T2_2, S1>::value>::type type;
		};
		
		template <template <class...> class Seq, class... T1, class T2_1, class T2_2, class... T2>
		struct ts_distinct<Seq<T1...>, Seq<T2_1, T2_2, T2...>, true>
		{
			private:
			typedef Seq<T1...> S1;
			
			public:
			typedef typename ts_distinct<S1, Seq<T2_2, T2...>, type_sequence_contains<T2_2, S1>::value>::type type;
		};
		
		
		template <template <class...> class Seq, class... T1, class T2_1>
		struct ts_distinct<Seq<T1...>, Seq<T2_1>, false>
		{
			typedef Seq<T1..., T2_1> type;
		};
		
		template <template <class...> class Seq, class... T1, class T2_1>
		struct ts_distinct<Seq<T1...>, Seq<T2_1>, true>
		{
			typedef Seq<T1...> type;
		};
	}
	}
	
	template <template <class...> class Seq, class... Types>
	struct type_sequence_distinct<Seq<Types...>>
	{
		typedef typename detail::distinct::ts_distinct<Seq<>, Seq<Types...>>::type type;
	};
	
	template <template <class...> class Seq>
	struct type_sequence_distinct<Seq<>>
	{
		typedef Seq<> type;
	};
	
	
	
	// type_sequence_remove<T<T...>, T>::type
	template <class, class, class = void>
	struct type_sequence_remove;
	
	template <template <class...> class Seq, class T>
	struct type_sequence_remove<Seq<>, T, void>
	{
		typedef Seq<> type;
		constexpr static bool value = false;
	};
	
	template <template <class...> class Seq, class... Types, class T>
	struct type_sequence_remove<Seq<Types...>, T, void> : public type_sequence_remove<Seq<>, T, Seq<Types...>>
	{};
	
	template <template <class...> class Seq, class... L, class R1, class... R, class T>
	struct type_sequence_remove<Seq<L...>, T, Seq<R1, R...>> : public type_sequence_remove<Seq<L..., R1>, T, Seq<R...>>
	{};
	
	template <template <class...> class Seq, class... L, class... R, class T>
	struct type_sequence_remove<Seq<L...>, T, Seq<T, R...>>
	{
		typedef Seq<L..., R...> type;
		constexpr static bool value = true;
	};
	
	template <template <class...> class Seq, class... L, class Y, class T>
	struct type_sequence_remove<Seq<L...>, T, Seq<Y>>
	{
		typedef Seq<L..., Y> type;
		constexpr static bool value = false;
	};
	
	template <template <class...> class Seq, class... L, class T>
	struct type_sequence_remove<Seq<L...>, T, Seq<T>>
	{
		typedef Seq<L...> type;
		constexpr static bool value = true;
	};
	
	
	
	// type_sequence_remove_all<T<T...>, T>::type
	template <class, class, bool = true>
	struct type_sequence_remove_all;
	
	template <template <class...> class Seq, class... Types, class T>
	struct type_sequence_remove_all<Seq<Types...>, T, true>
	{
		private:
		typedef type_sequence_remove<Seq<Types...>, T> __rem;
		
		public:
		constexpr static bool value = __rem::value;
		typedef typename type_sequence_remove_all<typename __rem::type, T, value>::type type;
	};
	
	template <template <class...> class Seq, class... Types, class T>
	struct type_sequence_remove_all<Seq<Types...>, T, false>
	{
		
		public:
		constexpr static bool value = false;
		typedef Seq<Types...> type;
	};
	
	
	
	// type_sequence_same_types<T<T...>, T<T...>>::value
	template <class, class, bool = true>
	struct type_sequence_same_types;
	
	template <template <class...> class Seq1, template <class...> class Seq2, class... T1, class... T2>
	struct type_sequence_same_types<Seq1<T1...>, Seq2<T2...>, false> : public Utils::integral_constant<bool, false>
	{
		
	};
	
	template <template <class...> class Seq1, template <class...> class Seq2>
	struct type_sequence_same_types<Seq1<>, Seq2<>, true> : public Utils::integral_constant<bool, true>
	{
		
	};
	
	/*template <template <class...> class Seq1, template <class...> class Seq2, class... T1>
	struct type_sequence_same_types<Seq1<T1...>, Seq2<T1...>, true> : public Utils::integral_constant<bool, true>
	{
		
	};*/
	
	template <template <class...> class Seq1, template <class...> class Seq2, class TL1, class... TL, class... TR>
	struct type_sequence_same_types<Seq1<TL1, TL...>, Seq2<TR...>, true> : public type_sequence_same_types<typename type_sequence_remove<Seq1<TL1, TL...>, TL1>::type, typename type_sequence_remove<Seq2<TR...>, TL1>::type, sizeof...(TL) == sizeof...(TR)-1>
	{
		
	};
	
	template <template <class...> class Seq1, template <class...> class Seq2, class TR1, class... TR, bool B>
	struct type_sequence_same_types<Seq1<>, Seq2<TR1, TR...>, B> : public Utils::integral_constant<bool, false>
	{
		
	};
	
	
	
	// type_sequence_for_each<T<T...>, Fn>::call(...)
	template <class, template <class...> class Fn>
	struct type_sequence_for_each;
	
	template <template <class...> class Seq, class T1, class... Types, template <class...> class Fn>
	struct type_sequence_for_each<Seq<T1, Types...>, Fn>
	{
		__attribute__((__always_inline__))
		static void call(auto... args)
		{
			Fn<T1>()(args...);
			type_sequence_for_each<Seq<Types...>, Fn>::call(args...);
		}
	};
	
	template <template <class...> class Seq, template <class...> class Fn>
	struct type_sequence_for_each<Seq<>, Fn>
	{
		__attribute__((__always_inline__))
		static void call(auto... args)
		{
			
		}
	};
	
	
	
	// type_sequence_for_each_until<T<T...>, Fn>::call(...)
	template <class, template <class...> class Fn>
	struct type_sequence_for_each_until;
	
	template <template <class...> class Seq, class T1, class... Types, template <class...> class Fn>
	struct type_sequence_for_each_until<Seq<T1, Types...>, Fn>
	{
		__attribute__((__always_inline__))
		static bool call(auto... args)
		{
			if (!Fn<T1>()(args...))
			{
				return type_sequence_for_each<Seq<Types...>, Fn>::call(args...);
			}
			return true;
		}
	};
	
	template <template <class...> class Seq, template <class...> class Fn>
	struct type_sequence_for_each_until<Seq<>, Fn>
	{
		__attribute__((__always_inline__))
		static bool call(auto... args)
		{
			return false;
		}
	};
	
	
	
	// type_sequence_repeat<T<T...>, N>::type
	template <class, unsigned N>
	struct type_sequence_repeat;
	
	template <template <class...> class Seq, class... Types, unsigned N>
	struct type_sequence_repeat<Seq<Types...>, N>
	{
		static_assert(N >= 0);
		typedef typename type_sequence_concat<Seq<Types...>, typename type_sequence_repeat<Seq<Types...>, N-1>::type>::type type;
	};
	
	template <template <class...> class Seq, class... Types>
	struct type_sequence_repeat<Seq<Types...>, 0>
	{
		typedef Seq<> type;
	};
	
	
	
	// type_sequence_equal<T<T...>, T<T...>>::value
	template <class, class>
	struct type_sequence_equal : public Utils::integral_constant<bool, false>
	{};
	
	template <template <class...> class Seq1, template <class...> class Seq2, class... Types>
	struct type_sequence_equal<Seq1<Types...>, Seq2<Types...>> : public Utils::integral_constant<bool, true>
	{};
	
	
	
	// type_sequence_sort<T, T<T...>>::type
	template <template <class...> class, class, bool = true> 
	struct type_sequence_sort;
	
	namespace detail
	{
		template <template <class...> class P, class, class, class...>
		struct __eval_pred;
		
		template <template <class...> class P, class L, class R, class... T>
		struct __eval_pred : public Utils::integral_constant<bool, P<L, R>::value || !P<R, L>::value>
		{};
		
		
		
		template <template <class...> class Predicate, class Types1, class Types2, bool B>
		struct __type_sequence_sort;
		
		template <template <class...> class P, template <class...> class Seq1, template <class...> class Seq2, class T1, class T2, class... Types, class... LTypes>
		struct __type_sequence_sort<P, Seq1<LTypes...>, Seq2<T1, T2, Types...>, true> : public __type_sequence_sort<P, Seq1<LTypes..., T1>, Seq2<T2, Types...>, __eval_pred<P, T2, Types...>::value>
		{};
		
		template <template <class...> class P, template <class...> class Seq1, template <class...> class Seq2, class T1, class T2, class... Types, class... LTypes>
		struct __type_sequence_sort<P, Seq1<LTypes...>, Seq2<T1, T2, Types...>, false> : public __type_sequence_sort<P, Seq1<LTypes..., T2>, Seq2<T1, Types...>, __eval_pred<P, T1, Types...>::value>
		{
			static_assert(P<T2, T1>::value);
		};
		
		
		template <template <class...> class P, template <class...> class Seq1, template <class...> class Seq2, class T1, class T2, class... LTypes>
		struct __type_sequence_sort<P, Seq1<LTypes...>, Seq2<T1, T2>, true>
		{
			typedef Seq1<LTypes..., T1, T2> type;
		};
		
		template <template <class...> class P, template <class...> class Seq1, template <class...> class Seq2, class T1, class T2, class... LTypes>
		struct __type_sequence_sort<P, Seq1<LTypes...>, Seq2<T1, T2>, false>
		{
			typedef Seq1<LTypes..., T2, T1> type;
		};
		
		
		
		
		
		
		template <template <class...> class Predicate, class Types1, class Types2>
		struct __type_sequence_sort_entry;
		
		template <template <class...> class P, template <class...> class Seq1, template <class...> class Seq2, class T1, class T2, class... Types>
		struct __type_sequence_sort_entry<P, Seq1<>, Seq2<T1, T2, Types...>> : public __type_sequence_sort<P, Seq1<>, Seq2<T1, T2, Types...>, P<T1, T2>::value>
		{};
		
		template <template <class...> class P, template <class...> class Seq1, template <class...> class Seq2, class T1>
		struct __type_sequence_sort_entry<P, Seq1<>, Seq2<T1>>
		{
			typedef Seq1<T1> type;
		};
		
		
		
		template <template <class...> class P, class Seq1, class Seq2, bool B = false>
		struct __type_sequence_sort_repeat;
		
		template <template <class...> class P, template <class...> class Seq, class... Types1, class... Types2>
		struct __type_sequence_sort_repeat<P, Seq<Types1...>, Seq<Types2...>, false>
		{
			private:
			typedef typename __type_sequence_sort_entry<P, Seq<>, Seq<Types2...>>::type next_seq;
			
			constexpr static bool next_equal = type_sequence_equal<Seq<Types2...>, next_seq>::value;
			
			public:
			
			typedef typename __type_sequence_sort_repeat<P, Seq<Types2...>, next_seq, next_equal>::type type;
		};
		
		template <template <class...> class P, template <class...> class Seq, class... Types1, class... Types2>
		struct __type_sequence_sort_repeat<P, Seq<Types1...>, Seq<Types2...>, true>
		{
			typedef Seq<Types2...> type;
		};
	}
	
	
	
	template <template <class...> class Predicate, template <class...> class Seq, class... T>
	struct type_sequence_sort<Predicate, Seq<T...>> : public detail::__type_sequence_sort_repeat<Predicate, Seq<T...>, Seq<T...>>
	{
		
	};
	
	
	
	// type_sequence_any<T, T<T...>>::value
	
	template <template <class...> class P, class Types, bool = false>
	struct type_sequence_any;
	
	template <template <class...> class P, template <class...> class Seq, class... Types>
	struct type_sequence_any<P, Seq<Types...>, true> : public Utils::integral_constant<bool, true>
	{
		
	};
	
	template <template <class...> class P, template <class...> class Seq, class T1, class... Types>
	struct type_sequence_any<P, Seq<T1, Types...>, false> : public type_sequence_any<P, Seq<Types...>, P<T1>::value>
	{
		
	};
	
	template <template <class...> class P, template <class...> class Seq>
	struct type_sequence_any<P, Seq<>, false> : public Utils::integral_constant<bool, false>
	{
		
	};
	
	
	
	
	// type_sequence_group_by<T, T<T...>>::type
	template <template <class...> class P, class>
	struct type_sequence_group_by;
	
	namespace detail
	{
		
		template <template <class...> class P, class T>
		struct predicate_grouper
		{
			template <class Y>
			struct inner : public Utils::integral_constant<bool, P<T, Y>::value>
			{
				
			};
		};
		
		template <template <class...> class P, class Types, class Groups>
		struct __type_sequence_group_by;
		
		template <template <class...> class P, class GroupsL, class T, class GroupsR, bool Fits>
		struct insert_group;
		
		template <template <class...> class P, template <class...> class Seq, class T, class... GroupsR, class... GroupsL, class... GroupTypes>
		struct insert_group<P, Seq<GroupsL...>, T, Seq<Seq<GroupTypes...>, GroupsR...>, true>
		{
			typedef Seq<GroupsL..., Seq<GroupTypes..., T>, GroupsR...> type;
		};
		
		template <template <class...> class P, template <class...> class Seq, class T, class... GroupsR, class... GroupsL, class... GroupTypes, class... GroupTypes_Next>
		struct insert_group<P, Seq<GroupsL...>, T, Seq<Seq<GroupTypes...>, Seq<GroupTypes_Next...>, GroupsR...>, false> : public insert_group<P, Seq<GroupsL..., Seq<GroupTypes...>>, T, Seq<Seq<GroupTypes_Next...>, GroupsR...>, type_sequence_any<predicate_grouper<P, T>::template inner, Seq<GroupTypes_Next...>>::value>
		{
			
		};
		
		template <template <class...> class P, template <class...> class Seq, class T, class... GroupsL, class... GroupTypes>
		struct insert_group<P, Seq<GroupsL...>, T, Seq<Seq<GroupTypes...>>, false>
		{
			typedef Seq<GroupsL..., Seq<GroupTypes...>, Seq<T>> type;
		};
		
		template <template <class...> class P, class T1, class... Types, template <class...> class Seq>
		struct __type_sequence_group_by<P, Seq<T1, Types...>, Seq<>> : public __type_sequence_group_by<P, Seq<Types...>, Seq<Seq<T1>>>
		{
			
		};
		
		template <template <class...> class P, class T1, class... Types, template <class...> class Seq, class... Groups, class... G1Types>
		struct __type_sequence_group_by<P, Seq<T1, Types...>, Seq<Seq<G1Types...>, Groups...>> : public __type_sequence_group_by<P, Seq<Types...>, typename insert_group<P, Seq<>, T1, Seq<Seq<G1Types...>, Groups...>, type_sequence_any<predicate_grouper<P, T1>::template inner, Seq<G1Types...>>::value>::type>
		{
			
		};
		
		template <template <class...> class P, template <class...> class Seq, class... Groups>
		struct __type_sequence_group_by<P, Seq<>, Seq<Groups...>>
		{
			typedef Seq<Groups...> type;
		};
	}
	
	template <template <class...> class P, template <class...> class Seq, class... Types>
	struct type_sequence_group_by<P, Seq<Types...>> : public detail::__type_sequence_group_by<P, Seq<Types...>, Seq<>>
	{
		
	};
	
	
	
	// type_sequence_apply_templates<T, S<T...>>::type
	template <class, template <class...> class>
	struct type_sequence_apply_templates;
	
	template <template <class...> class Seq, template <class...> class T, class... Types>
	struct type_sequence_apply_templates<Seq<Types...>, T>
	{
		typedef Seq<T<Types>...> type;
	};
	
	
	
	// type_sequence_reverse<S<T...>>::type
	template <class>
	struct type_sequence_reverse;
	
	template <template <class...> class Seq, class T1, class... T>
	struct type_sequence_reverse<Seq<T1, T...>>
	{
		typedef typename type_sequence_append<typename type_sequence_reverse<Seq<T...>>::type, T1>::type type;
	};
	
	template <template <class...> class Seq>
	struct type_sequence_reverse<Seq<>>
	{
		typedef Seq<> type;
	};
	
	
	
	template <class S1, class S2, class Y = void, class I = void, bool p1 = true>
	struct type_sequence_intersect;
	
	template <template <class...> class Seq1, template <class...> class Seq2, class Y, class... T1, class... T2>
	struct type_sequence_intersect<Seq1<Y, T1...>, Seq2<T2...>, void, void, true> : public type_sequence_intersect<Seq1<T1...>, Seq2<T2...>, Y, Seq1<>, type_sequence_contains<Y, Seq2<T2...>>::value>
	{};
	
	template <template <class...> class Seq1, template <class...> class Seq2, class... T2>
	struct type_sequence_intersect<Seq1<>, Seq2<T2...>, void, void, true>
	{
		typedef Seq1<> type;
	};
	
	template <template <class...> class Seq1, template <class...> class Seq2, class H, class Y, class... T1, class... T2, class... I>
	struct type_sequence_intersect<Seq1<H, T1...>, Seq2<T2...>, Y, Seq1<I...>, true> : public type_sequence_intersect<Seq1<T1...>, typename type_sequence_remove<Seq2<T2...>, Y>::type, H, Seq1<I..., Y>, type_sequence_contains<H, typename type_sequence_remove<Seq2<T2...>, Y>::type>::value>
	{};
	
	template <template <class...> class Seq1, template <class...> class Seq2, class H, class Y, class... T1, class... T2, class... I>
	struct type_sequence_intersect<Seq1<H, T1...>, Seq2<T2...>, Y, Seq1<I...>, false> : public type_sequence_intersect<Seq1<T1...>, Seq2<T2...>, H, Seq1<I...>, type_sequence_contains<H, Seq2<T2...>>::value>
	{};
	
	template <template <class...> class Seq1, template <class...> class Seq2, class Y, class... T2, class... I>
	struct type_sequence_intersect<Seq1<>, Seq2<T2...>, Y, Seq1<I...>, true>
	{
		typedef Seq1<I..., Y> type;
	};
	
	template <template <class...> class Seq1, template <class...> class Seq2, class Y, class... T2, class... I>
	struct type_sequence_intersect<Seq1<>, Seq2<T2...>, Y, Seq1<I...>, false>
	{
		typedef Seq1<I...> type;
	};
	
	
	
	// type_sequence_indexes<S<T...>, S<T...>>::type
	template <class, class, class = number_sequence<size_t>>
	struct type_sequence_indexes;
	
	template <template <class...> class Seq1, template <class...> class Seq2, class... TLeft, class Y, class... TRight, template <class N, N... I> class ISeq, size_t... I>
	struct type_sequence_indexes<Seq1<TLeft...>, Seq2<Y, TRight...>, ISeq<size_t, I...>> : public type_sequence_indexes<Seq1<TLeft...>, Seq2<TRight...>, ISeq<size_t, I..., indexof_type<Y, Seq1<TLeft...>>::value>>
	{
		
	};
	
	template <template <class...> class Seq1, template <class...> class Seq2, template <class N, N... I> class ISeq, class... TLeft, size_t... I>
	struct type_sequence_indexes<Seq1<TLeft...>, Seq2<>, ISeq<size_t, I...>>
	{
		typedef ISeq<size_t, I...> type;
	};
	
	
	
	// type_sequence_skip<S<T...>, N>::type
	template <class, size_t N, bool = (N >= 1)>
	struct type_sequence_skip;
	
	template <template <class...> class Seq, class T, class... Types, size_t N>
	struct type_sequence_skip<Seq<T, Types...>, N, true>
	{
		static_assert(N > 0);
		typedef typename type_sequence_skip<Seq<Types...>, N-1, (N-1 > 0)>::type type;
	};
	
	template <template <class...> class Seq, class T, size_t N>
	struct type_sequence_skip<Seq<T>, N, true>
	{
		static_assert(N == 0, "Index too high for type list");
		typedef Seq<T> type;
	};
	
	template <template <class...> class Seq, class... Types>
	struct type_sequence_skip<Seq<Types...>, 0, false>
	{
		typedef Seq<Types...> type;
	};
	
	template <template <class...> class Seq>
	struct type_sequence_skip<Seq<>, 0, false>
	{
		typedef Seq<> type;
	};
	
	template <template <class...> class Seq, class T, class... Types>
	struct type_sequence_skip<Seq<T, Types...>, 1, true>
	{
		typedef Seq<Types...> type;
	};
	
	
	
	// type_sequence_first<S<T...>>::type
	template <class>
	struct type_sequence_first;
	
	template <template <class...> class Seq, class T, class... Types>
	struct type_sequence_first<Seq<T, Types...>>
	{
		typedef T type;
	};
	
	
	
	// type_sequence_at<S<T...>, N>::type
	template <class, size_t>
	struct type_sequence_at;
	
	template <template <class...> class Seq, class... Types, size_t N>
	struct type_sequence_at<Seq<Types...>, N>
	{
		typedef typename type_sequence_first<typename type_sequence_skip<Seq<Types...>, N>::type>::type type;
	};
	
	
	
	// type_sequence_index_of_type_index<S<T...>>::value(Utils::type_index)
	template <class, size_t = 0>
	struct type_sequence_index_of_type_index;
	
	template <template <class...> class Seq, class T1, class... Types, size_t N>
	struct type_sequence_index_of_type_index<Seq<T1, Types...>, N>
	{
		constexpr static size_t value(const Utils::type_index& inf)
		{
			if (inf == typeid(T1))
			{
				return N;
			}
			else
			{
				return type_sequence_index_of_type_index<Seq<Types...>, N+1>::value(inf);
			}
		}
	};
	
	template <template <class...> class Seq, class T1, size_t N>
	struct type_sequence_index_of_type_index<Seq<T1>, N>
	{
		constexpr static size_t value(const Utils::type_index& inf)
		{
			if (inf == typeid(T1))
			{
				return N;
			}
			else
			{
				return indexof_type<void, type_sequence<>>::value;
			}
		}
	};
	
	// type_sequence_try_call_by_type_index<S<T...>, Fn>::call(Utils::type_index, auto...) -> bool
	template <class, template <class...> class>
	struct type_sequence_try_call_by_type_index;
	
	template <template <class...> class Seq, class T1, class... Types, template <class...> class Fn>
	struct type_sequence_try_call_by_type_index<Seq<T1, Types...>, Fn>
	{
		template <class... Args>
		__attribute__((__always_inline__))
		constexpr static bool call(Utils::type_index inf, Args... args)
		{
			if (inf == typeid(T1))
			{
				Fn<T1> fn;
				fn(Utils::forward<Args>(args)...);
				return true;
			}
			else
			{
				return type_sequence_try_call_by_type_index<Seq<Types...>, Fn>::template call<Args...>(inf, Utils::forward<Args>(args)...);
			}
		}
	};
	
	template <template <class...> class Seq, class T1, template <class...> class Fn>
	struct type_sequence_try_call_by_type_index<Seq<T1>, Fn>
	{
		template <class... Args>
		__attribute__((__always_inline__))
		constexpr static bool call(Utils::type_index inf, Args... args)
		{
			if (inf == typeid(T1))
			{
				Fn<T1> fn;
				fn(Utils::forward<Args>(args)...);
				return true;
			}
			else
			{
				return false;
			}
		}
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	template <class... Types>
	struct type_sequence
	{
		private:
		typedef type_sequence<Types...> self_t;
		
		public:
		
		constexpr static auto size = sizeof...(Types);
		constexpr static size_t npos = indexof_type<void, type_sequence<>>::npos;
		
		
		/*template <class T>
		struct indexof : public indexof_type<T, type_sequence<Types...>>
		{
			
		};*/
		
		template <class T>
		constexpr static auto indexof = indexof_type<T, type_sequence<Types...>>::value;
		
		
		/*template <class T>
		struct contains : public type_sequence_contains<T, type_sequence<Types...>>
		{
			
		};*/
		
		template <class T>
		using append = type_sequence<Types..., T>;
		
		template <class T>
		constexpr static bool contains = type_sequence_contains<T, type_sequence<Types...>>::value;
		
		
		/*template <template <class...> class T>
		struct where : public type_sequence_filter<T, type_sequence<Types...>>
		{
			
		};*/
		
		template <template <class...> class T>
		using where = typename type_sequence_filter<T, self_t>::type;
		
		/*template <template <class...> class T>
		struct apply_transforms
		{
			typedef type_sequence<typename T<Types>::type...> type;
		};*/
		
		template <template <class...> class T>
		using apply_transforms = type_sequence<typename T<Types>::type...>;
		
		/*template <template <class...> class T>
		struct apply_templates
		{
			typedef type_sequence<T<Types>...> type;
		};*/
		
		template <template <class...> class T>
		using apply_templates = type_sequence<T<Types>...>;
		
		/*template <template <class...> class T>
		struct apply_template
		{
			typedef T<Types...> type;
		};*/
		
		
		template <template <class...> class T>
		using apply_template = T<Types...>;
		
		
		struct member_typedefs
		{
			typedef type_sequence<typename Types::type...> type;
		};
		
		/*template <class... T>
		struct concat : public type_sequence_concat<type_sequence<Types...>, T...>
		{ };*/
		
		template <class... T>
		using concat = typename type_sequence_concat<type_sequence<Types...>, T...>::type;
		
		/*template <template <class...> class T>
		struct select_many : public type_sequence_select_many<T, type_sequence<Types...>>
		{
			
		};*/
		
		template <template <class...> class T>
		using select_many = typename type_sequence_select_many<T, type_sequence<Types...>>::type;
		
		template <template <class...> class T>
		using select = type_sequence<typename T<Types>::type...>;
		
		/*struct distinct
		{
			typedef typename type_sequence_distinct<type_sequence<Types...>>::type type;
		};*/
		
		typedef typename type_sequence_distinct<type_sequence<Types...>>::type distinct;
		
		template <class T>
		using remove = typename type_sequence_remove<type_sequence<Types...>, T>::type;
		
		template <class T>
		using remove_all = typename type_sequence_remove_all<type_sequence<Types...>, T>::type;
		
		
		template <class T>
		constexpr static bool same_types = type_sequence_same_types<type_sequence<Types...>, T>::value;
		
		
		template <template <class...> class Fn>
		__attribute__((__always_inline__))
		static void for_each(auto... args)
		{
			type_sequence_for_each<type_sequence<Types...>, Fn>::call(args...);
		}
		
		template <template <class...> class Fn>
		__attribute__((__always_inline__))
		static bool for_each_until(auto... args)
		{
			return type_sequence_for_each_until<type_sequence<Types...>, Fn>::call(args...);
		}
		
		
		template <unsigned N>
		using repeat = typename type_sequence_repeat<self_t, N>::type;
		
		
		template <class Seq2>
		constexpr static bool equals = type_sequence_equal<self_t, Seq2>::value;
		
		
		template <template <class...> class Predicate>
		using sort = typename type_sequence_sort<Predicate, self_t>::type;
		
		template <template <class...> class Predicate>
		constexpr static bool any = type_sequence_any<Predicate, self_t>::value;
		
		template <template <class...> class Predicate>
		using group_by = typename type_sequence_group_by<Predicate, self_t>::type;
		
		typedef typename type_sequence_reverse<self_t>::type reverse;
		
		template <class S>
		using intersect = typename type_sequence_intersect<self_t, S>::type;
		
		
		
		template <class S>
		using indexes = typename type_sequence_indexes<self_t, S>::type;
		
		
		template <template <class...> class T>
		constexpr static auto select_value = Utils::make_tuple(T<Types>::value...);
		
		
		template <class T = Utils::type_index>
		constexpr static auto typeids = Utils::make_tuple(T(typeid(Types))...);
		
		
		
		template <size_t N>
		using type_at = typename type_sequence_at<self_t, N>::type;
		
		template <size_t... N>
		using types_at = type_sequence<type_at<N>...>;
		
		
		constexpr static size_t index_of_type_index(Utils::type_index i)
		{
			return type_sequence_index_of_type_index<self_t>::value(i);
		}
		
		
		template <template <class...> class Fn, class... Args>
		__attribute__((__always_inline__))
		constexpr static bool try_call_by_type_index(Utils::type_index inf, Args... args)
		{
			return type_sequence_try_call_by_type_index<self_t, Fn>::template call<Args...>(inf, Utils::forward<Args>(args)...);
		}
		
	};
	
	
	
	
	
	
	
	
	
	
	
	template <class N, N... Values>
	struct number_sequence
	{
		private:
		typedef number_sequence<N, Values...> self_t;
		
		public:
		typedef N value_type;
		typedef Utils::integer_sequence<N, Values...> utils_type;
		
		
		
		constexpr static auto size = sizeof...(Values);
		constexpr static size_t npos = indexof_type<void, type_sequence<>>::npos;
		
		
		template <class... T>
		constexpr static auto get_tuple_values(Utils::tuple<T...>& tup)
		{
			return Utils::make_tuple(Utils::get<Values>(tup)...);
		}
		
		template <class... T>
		constexpr static auto get_tuple_pointers(Utils::tuple<T...>& tup)
		{
			return Utils::make_tuple(&Utils::get<Values>(tup)...);
		}
	};
	
	
}
}

#endif