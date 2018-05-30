#ifndef INCLUDED_SUPPORT_SQLITE_ENTITYFACTORY_HH
#define INCLUDED_SUPPORT_SQLITE_ENTITYFACTORY_HH

#include "serializer.hh"
#include "entity.hh"

namespace Support { namespace SQLite
{
	
	template <class Context>
	class BaseEntityFactory
	{
		public:
		typedef typename Utils::decay<Context>::type context_type;
		
		
		
		virtual ~BaseEntityFactory() = default;
	};
	
	
	
	template <class T, class Context, bool Abstract = Utils::is_abstract<T>::value, bool Poly = Utils::is_polymorphic<T>::value, class B = typename detail::BuiltModel<T>::Bases>
	class EntityFactory;
	
	template <class T, class Context, class... B>
	class EntityFactory<T, Context, false, false, base_types<B...>> : public virtual BaseEntityFactory<Context>, public EntityFactory<B, Context>...
	{
		private:
		typedef typename Meta::type_sequence<Utils::type_index>::repeat<sizeof...(B) + 1>::template apply_template<Utils::tuple> type_ids_type;
		
		
		
		public:
		typedef T entity_type;
		using typename BaseEntityFactory<Context>::context_type;
		
		protected:
		
		public:
		static type_ids_type type_ids;
		
		virtual T* make()
		{
			return new T;
		}
		
	};
	
	
	
	
	template <class T, class Context, class... B>
	class EntityFactory<T, Context, true, true, base_types<B...>> : public virtual BaseEntityFactory<Context>, public EntityFactory<B, Context>...
	{
		private:
		typedef typename Meta::type_sequence<Utils::type_index>::repeat<sizeof...(B) + 1>::template apply_template<Utils::tuple> type_ids_type;
		
		
		
		public:
		typedef T entity_type;
		using typename BaseEntityFactory<Context>::context_type;
		
		
		protected:
		
		public:
		static type_ids_type type_ids;
		
		virtual T* make() = 0;
		virtual T* make_proxy(context_type&) = 0;
		
	};
	
	
	template <class T, class Context, class... B>
	class EntityFactory<T, Context, false, true, base_types<B...>> : public virtual BaseEntityFactory<Context>, public EntityFactory<B, Context>...
	{
		private:
		typedef typename Meta::type_sequence<Utils::type_index>::repeat<sizeof...(B) + 1>::template apply_template<Utils::tuple> type_ids_type;
		
		
		
		public:
		typedef T entity_type;
		using typename BaseEntityFactory<Context>::context_type;
		
		
		protected:
		
		public:
		static type_ids_type type_ids;
		
		virtual T* make()
		{
			return new T;
		}
		
		virtual T* make_proxy(context_type& c)
		{
			return new ContextEntityProxy<T, Context>(c);
		}
		
	};
	
	
	
	
	template <class T, class Context, class... B>
	typename EntityFactory<T, Context, false, false, base_types<B...>>::type_ids_type EntityFactory<T, Context, false, false, base_types<B...>>::type_ids(Utils::type_index(typeid(T)), Utils::type_index(typeid(B))...);
	
	template <class T, class Context, class... B>
	typename EntityFactory<T, Context, true, true, base_types<B...>>::type_ids_type EntityFactory<T, Context, true, true, base_types<B...>>::type_ids(Utils::type_index(typeid(T)), Utils::type_index(typeid(B))...);
	
	template <class T, class Context, class... B>
	typename EntityFactory<T, Context, false, true, base_types<B...>>::type_ids_type EntityFactory<T, Context, false, true, base_types<B...>>::type_ids(Utils::type_index(typeid(T)), Utils::type_index(typeid(B))...);
	
}
}

#endif