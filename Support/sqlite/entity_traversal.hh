#ifndef INCLUDED_SUPPORT_SQLITE_ENTITY_TRAVERSAL_HH
#define INCLUDED_SUPPORT_SQLITE_ENTITY_TRAVERSAL_HH

#include <Common.h>
#include "serializer.hh"
#include <Support/MetaProgramming/Utilities.hh>
#include <Utils/unordered_set>
#include <Utils/list>
#include <iostream>


#if __cplusplus >= 201703L


namespace Support { namespace SQLite
{
	
	namespace detail
	{
		
		template <class T, class Alloc = Utils::allocator<T>>
		struct traversal_container
		{
			static_assert(Utils::is_pointer<T>::value);
			Utils::list<T, Alloc> list;
			Utils::unordered_set<T, Utils::hash<T>, Utils::equal_to<T>, Alloc> set;
			
			void add(T item)
			{
				if (set.count(item) == 0)
				{
					list.push_back(item);
				}
			}
			
		};
		
		
		
		
		template <unsigned N, class Context, class Sets, template <class...> class sub_traverser, unsigned Max>
		struct traverser_looper;
		
		
		template <unsigned N, class Context, template <class...> class _Sets, template <class...> class sub_traverser, class... Set_Types, unsigned M>
		struct traverser_looper<N, Context, _Sets<Set_Types...>, sub_traverser, M>
		{
			typedef _Sets<Set_Types...> Sets;
			
			__attribute__((__always_inline__))
			static bool call(Context& c, Utils::shared_ptr<Sets> sets)
			{
				typedef typename Utils::remove_pointer<typename Utils::decay<decltype(Utils::get<N>(sets->sets).set)>::type::value_type>::type T;
				
				sub_traverser<T, Sets> trav(c, sets);
				bool changed = false;
				auto& container = sets->template set<T>();
				if (container.list.size() > 0)
				{
					
					decltype(container.list) list(Utils::move(container.list));
					
					auto it = list.begin();
					while (it != list.end())
					{
						if (container.set.count(*it) != 0)
						{
							it = list.erase(it);
						}
						else
						{
							container.set.insert(*it);
							++it;
						}
					}
					
					for (auto item : list)
					{
						trav.visit(item);
					}
					
					changed = true;
				}
				
				changed |= traverser_looper<N+1, Context, Sets, sub_traverser, M>::call(c, sets);
				return changed;
			}
		};
		
		template <unsigned N, class Context, template <class...> class _Sets, template <class...> class sub_traverser, class... Set_Types>
		struct traverser_looper<N, Context, _Sets<Set_Types...>, sub_traverser, N>
		{
			typedef _Sets<Set_Types...> Sets;
			__attribute__((__always_inline__))
			static bool call(Context& c, Utils::shared_ptr<Sets> sets)
			{
				return false;
			}
		};
	}
	
	
	
	
	class Traverser
	{
		public:
		
		private:
		
		public:
		
		template <class...>
		struct sets_type;
		
		template <class T>
		using tuple_for =
			typename detail::all_related_entity_types<T>::type
			::template apply_transforms<Utils::add_pointer>
			::template apply_templates<detail::traversal_container>
			::template apply_template<Utils::tuple>;
		
		template <class T>
		using sets_type_for =
			typename detail::all_related_entity_types<T>::type
			::template apply_template<sets_type>;
		
		
		template <class... T>
		struct sets_type
		{
			private:
			typedef typename
				Meta::type_sequence<T...>
				::template apply_transforms<Utils::add_pointer>
				::template apply_templates<detail::traversal_container>
				::template apply_template<Utils::tuple>
			sets_tuple_type;
			
			public:
			constexpr static size_t count = sizeof...(T);
			
			sets_tuple_type sets;
			//typename Meta::type_sequence<size_t>::repeat<sizeof...(T)>::template apply_template<Utils::tuple> sizes;
			
			~sets_type() = default;
			
			template <class Y>
			constexpr static size_t indexof = Meta::type_sequence<T...>::template indexof<Y>;
			
			template <class Y>
			detail::traversal_container<Y*>& set()
			{
				constexpr size_t index = indexof<Y>;
				static_assert(index < sizeof...(T));
				return Utils::get<index>(sets);
			}
			
			/*template <class Y>
			size_t& size()
			{
				constexpr size_t index = indexof<Y>;
				static_assert(index < sizeof...(T));
				return Utils::get<index>(sizes);
			}*/
		};
		
		
		
		template <class T, class Entities_Sets = sets_type_for<T>>
		struct type_traverser
		{
			static_assert(is_entity<T>::value);
			private:
			
			constexpr static size_t type_count = Entities_Sets::count;
			
			
			public:
			typedef Entities_Sets sets_type;
			typedef Utils::shared_ptr<Entities_Sets> Sets_Ptr;
			
			
			Traverser& traverser;
			Sets_Ptr sets;
			
			type_traverser(Traverser& t, Sets_Ptr sets) : traverser(t), sets(sets)
			{
				
			}
			
			type_traverser(Traverser& t) : type_traverser(t, Utils::make_shared<Entities_Sets>())
			{
				
			}
			
			
			void visit(T* ptr)
			{
				if (!ptr)
				{
					return;
				}
				
				typedef detail::BuiltModel<T> Model;
				constexpr auto store_fn = Model::template store<type_traverser<T, Entities_Sets>>;
				
				constexpr size_t T_index = Entities_Sets::template indexof<T>;
				auto& container = Utils::get<T_index>(sets->sets);
				
				
				
				container.set.insert(ptr);
				store_fn(ptr, *this);
			}
			
			template <class Y>
			bool store(const char* n, const Y& y)
			{
				//std::cout << n << "\n";
				return true;
			}
			
			template <class Y>
			bool store(Y* obj)
			{
				if (obj)
				{
					constexpr size_t Y_index = Entities_Sets::template indexof<Y>;
					auto& container = Utils::get<Y_index>(sets->sets);
					container.add(obj);
				}
				return true;
			}
			
		};
		
		
		private:
		
		public:
		
		
		
		
		template <class T>
		Utils::shared_ptr<sets_type_for<T>> traverse(T* ptr)
		{
			type_traverser<T> trav(*this);
			trav.visit(ptr);
			assert(trav.sets);
			
			typedef sets_type_for<T> sets_type;
			
			while (detail::traverser_looper<0, Traverser, sets_type, type_traverser, sets_type::count>::call(*this, trav.sets))
			{
				
			}
			
			
			
			return trav.sets;
		}
		
	};
}
}

#endif
#endif