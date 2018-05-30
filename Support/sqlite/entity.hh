#ifndef INCLUDED_SUPPORT_SQLITE_ENTITY_HH
#define INCLUDED_SUPPORT_SQLITE_ENTITY_HH

#include <Common.h>

namespace Support { namespace SQLite
{
	
	class Entity
	{
		protected:
		
		
		public:
		
		virtual const char* model_name() const = 0;
		
		
	};
	
	class EntityWithContext : public virtual Entity
	{
		virtual bool store(bool includeRelated) const = 0;
		
		virtual bool load(bool includeRelated) = 0;
	};
	
	template <class H>
	class ContextEntity : public EntityWithContext
	{
		public:
		typedef typename Utils::decay<H>::type context_type;
		
		protected:
		context_type* context;
		
		public:
		ContextEntity(context_type& c) : context(&c)
		{
			
		}
		
	};
	
	
	template <class T>
	class EntityProxy : public T, public virtual Entity
	{
		static_assert(Utils::is_polymorphic<T>::value);
		static_assert(!Utils::is_abstract<T>::value);
		static_assert(is_entity<T>::value);
		
		public:
		typedef T entity_type;
		typedef detail::BuiltModel<entity_type> model_type;
		typedef typename model_has_key<entity_type>::key_type key_type;
		
		protected:
		
		
		private:
		
		
		
		public:
		constexpr static auto key_name = model_has_key<entity_type>::key.name;
		constexpr static auto& members = model_type::members;
		const key_type& key;
		
		template <class... Args>
		EntityProxy(Args&&... args) : T(Utils::forward<Args&&>(args)...), Entity(), key(((entity_type*)this)->*model_has_key<entity_type>::key.member)
		{
			
		}
		
		virtual const char* model_name() const override
		{
			return model_type::name;
		}
		
		template <class ContextView>
		void store_context(ContextView& view) const
		{
			view.store(key_name, key);
			
		}
		
		void set_key(const key_type& k)
		{
			((entity_type*)this)->*model_has_key<entity_type>::key.member = k;
		}
		
		void set_key(key_type&& k)
		{
			((entity_type*)this)->*model_has_key<entity_type>::key.member = k;
		}
	};
	
	template <class T, class H>
	class ContextEntityProxy : public EntityProxy<T>, public ContextEntity<H>
	{
		private:
		typedef EntityProxy<T> proxy_base;
		typedef ContextEntity<H> context_base;
		
		public:
		using typename proxy_base::entity_type;
		using typename context_base::context_type;
		
		protected:
		
		
		public:
		
		template <class... Args>
		ContextEntityProxy(context_type& context, Args&&... args) : EntityProxy<T>(Utils::forward<Args&&>(args)...), ContextEntity<H>(context)
		{
			
		}
		
		virtual bool store(bool includeRelated) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual bool load(bool includeRelated) override
		{
			assert(NOT_IMPLEMENTED);
		}
	};
	
	
	
	
	
	
	
	template <class T>
	struct supports_proxy : public Utils::integral_constant<bool, Utils::is_polymorphic<T>::value && !Utils::is_abstract<T>::value && is_entity<T>::value>
	{};
}
}

#endif