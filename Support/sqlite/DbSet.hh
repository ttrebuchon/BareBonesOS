#if __cplusplus >= 201703L
#ifndef INCLUDED_SUPPORT_SQLITE_DBSET_HH
#define INCLUDED_SUPPORT_SQLITE_DBSET_HH

#include <Common.h>
#include <Utils/memory>
#include <Utils/unordered_set>
#include <Utils/utility>

#include "serializer.hh"
#include "entity.hh"


#include <iostream>




namespace Support { namespace SQLite
{
	
	template <class TEntity, class Allocator = Utils::allocator<typename Utils::decay<TEntity>::type>, bool Abstract = Utils::is_abstract<TEntity>::value>
	class DbSet;
	
	namespace detail
	{
		template <class T, class K>
		struct entity_key_selector
		{
			typedef T type;
			typedef K key_type;
			
			private:
			
			constexpr static key_type __default_key = key_type();
			
			public:
			
			constexpr const key_type& operator()(const T* obj) const noexcept
			{
				if (!obj)
				{
					return __default_key;
				}
				assert(obj);
				return obj->*model_has_key<T>::key.member;
			}
			
		};
		
		template <class TEntity, class Context, class Deriveds, class Allocator = Utils::allocator<typename Utils::decay<TEntity>::type>>
		class _DbSet;
		
		template <class TEntity, class Context, class Deriveds, class Allocator = Utils::allocator<typename Utils::decay<TEntity>::type>>
		class _AbstractDbSet;
		
		template <class TEntity, bool b = !Utils::is_abstract<TEntity>::value>
		struct dbset_chooser;
		
		template <class T>
		struct dbset_chooser<T, true>
		{
			template <class... Y>
			struct inner
			{
				typedef _DbSet<T, Y...> type;
			};
		};
		
		template <class T>
		struct dbset_chooser<T, false>
		{
			template <class... Y>
			struct inner
			{
				typedef _AbstractDbSet<T, Y...> type;
			};
		};
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		template <class TEntity, class Allocator>
		class _DbSet<TEntity, void, void, Allocator>
		{
			public:
			typedef typename Utils::decay<TEntity>::type entity_type;
			typedef entity_type* value_type;
			typedef const entity_type* const_value_type;
			typedef typename Allocator::template rebind<entity_type>::other allocator_type;
			typedef size_t size_type;
			typedef typename model_has_key<entity_type>::key_type key_type;
			typedef Utils::hash_table<key_type, value_type, entity_key_selector<entity_type, key_type>, Utils::hash<key_type>, Utils::equal_to<key_type>, allocator_type> key_container_type;
			typedef Utils::hash_table<const_value_type, value_type, Utils::SelectIdentity, Utils::hash<const_value_type>, Utils::equal_to<const_value_type>, allocator_type> entity_container_type;
			
			protected:
			key_container_type key_container;
			entity_container_type entity_container;
			key_container_type staging_container;
			Utils::unordered_set<value_type, Utils::hash<value_type>, Utils::equal_to<value_type>, allocator_type> owned_objects;
			allocator_type alloc;
			
			public:
			const bool hasDerived;
			
			_DbSet(const bool hasDerived, const allocator_type& alloc = allocator_type(), size_t bucketHint = 10);
			
			
			virtual ~_DbSet()
			{
				for (auto item : owned_objects)
				{
					if (item)
					{
						delete item;
					}
				}
				
				owned_objects.clear();
			}
			
			
			
			allocator_type get_allocator() const
			{
				return alloc;
			}
			
			template <class Y>
			Y* create_local(const key_type& key) const
			{
				Y* y = nullptr;
				if constexpr (supports_proxy<Y>::value)
				{
					
				}
				else
				{
					typename allocator_type::template rebind<Y>::other a(get_allocator());
					y = a.allocate(1);
					a.construct(y);
					auto t = (entity_type*)y;
					auto k = model_has_key<entity_type>::get_key(t);
					assert(k);
					*k = key;
				}
				return y;
			}
			
			
			
			
			void attach(value_type v)
			{
				bool created;
				entity_container.getCreate(v, &created);
				if (created)
				{
					key_container.getCreate(v, &created);
					if (!created)
					{
						key_container.rehash(key_container.bucketsSize());
						key_container.getCreate(v, &created);
						assert(created);
					}
				}
				else
				{
					key_container.getCreate(v, &created);
					if (created)
					{
						key_container.rehash(key_container.bucketsSize());
					}
				}
				
			}
			
			bool detach(value_type item)
			{
				bool found = false;
				if (owned_objects.count(item))
				{
					owned_objects.erase(item);
					found = true;
				}
				
				if (entity_container.get(item))
				{
					entity_container.erase(item);
					found = true;
				}
				
				auto key = detail::BuiltModel<entity_type>::get_key(item);
				assert(key);
				auto node = key_container.get(*key);
				if (node)
				{
					if (node->value == item)
					{
						key_container.erase(*key);
						found = true;
					}
					else
					{
						key_container.rehash(key_container.bucketsSize());
					}
				}
				
				node = staging_container.get(*key);
				
				if (node)
				{
					if (node->value == item)
					{
						staging_container.erase(node);
						found = true;
					}
					else
					{
						staging_container.rehash(staging_container.bucketsSize());
					}
				}
				
				return found;
			}
			
			bool persist(value_type item)
			{
				if (owned_objects.count(item))
				{
					owned_objects.erase(item);
					return true;
				}
				else
				{
					return false;
				}
			}
			
			bool lapse(value_type item)
			{
				if (owned_objects.count(item))
				{
					return true;
				}
				else if (count_local(item))
				{
					owned_objects.insert(item);
					return true;
				}
				else if (count_staging(item))
				{
					owned_objects.insert(item);
					return true;
				}
				
				return false;
			}
			
			bool owns(value_type item) const
			{
				return owned_objects.count(item);
			}
			
			bool unload(value_type item)
			{
				bool owned = false;
				if (owned_objects.count(item))
				{
					owned = true;
				}
				
				if (detach(item))
				{
					if (owned)
					{
						alloc.destroy(item);
						alloc.deallocate(item, 1);
					}
					return true;
				}
				
				return false;
			}
			
			value_type get_local(const key_type& k)
			{
				auto n = key_container.get(k);
				if (n)
				{
					return n->value;
				}
				else
				{
					return nullptr;
				}
			}
			
			value_type get_staging(const key_type& k)
			{
				auto n = staging_container.get(k);
				if (n)
				{
					return n->value;
				}
				else
				{
					return nullptr;
				}
			}
			
			size_type size_local() const noexcept
			{
				return entity_container.size();
			}
			
			size_type size_staging() const noexcept
			{
				return staging_container.size();
			}
			
			size_type count_local(const key_type& k) const
			{
				return key_container.get(k) != nullptr;
			}
			
			size_type count_local(const_value_type e) const
			{
				if (e)
				{
					if (entity_container.get(e))
					{
						return 1;
					}
				}
				
				return 0;
			}
			
			size_type count_staging(const key_type& k) const
			{
				return staging_container.get(k) != nullptr;
			}
			
			size_type count_staging(const_value_type e) const
			{
				if (e)
				{
					auto key = detail::BuiltModel<entity_type>::get_key(e);
					
					if (key)
					{
						if (staging_container.get(*key))
						{
							return 1;
						}
					}
				}
				
				return 0;
			}
			
			int commit()
			{
				int count = 0;
				for (auto& item : staging_container)
				{
					this->attach(item);
					++count;
				}
				staging_container.clear();
				return count;
			}
			
			bool push_back(value_type item, bool owned = false)
			{
				
				bool created;
				if (true)//!count_local(item))
				{
					staging_container.getCreate(item, &created);
					if (created)
					{
						if (owned)
						{
							owned_objects.insert(item);
						}
						return true;
					}
				}
				if (owned && item)
				{
					delete item;
				}
				return false;
			}
			
			virtual bool count_remote(const key_type&) const = 0;
			virtual bool count_remote(key_type&&) const = 0;
			virtual size_type size_remote() const = 0;
			virtual value_type get_remote(const key_type&) = 0;
			virtual int push() = 0;
			virtual int apply_changes() = 0;
			virtual TEntity* construct_new(const key_type& k) = 0;
			virtual entity_type* find_derived_local(const Utils::type_index&, const key_type&) = 0;
			virtual entity_type* find_derived_local(const key_type&) = 0;
			virtual Utils::type_index get_type_for(const key_type&) const = 0;
		};
		
		
		
		
		
		
		
		
		
		
		
		
		template <class TEntity, class Allocator, template <class...> class Seq, class... Derived>
		class _DbSet<TEntity, void, Seq<Derived...>, Allocator> : public _DbSet<TEntity, void, void, Allocator>
		{
			public:
			typedef typename Utils::decay<TEntity>::type entity_type;
			typedef typename Allocator::template rebind<entity_type>::other allocator_type;
			using typename _DbSet<TEntity, void, void, Allocator>::key_type;
			
			
			protected:
			
			Utils::tuple<DbSet<Derived>*...> derived_sets;
			
			
			template <class Y>
			struct get_derived
			{
				constexpr get_derived() = default;
				
				constexpr void operator()(_DbSet<TEntity, void, Seq<Derived...>, Allocator>* _this, const key_type& key, entity_type** ent) const
				{
					constexpr size_t index = Meta::type_sequence<Derived...>::template indexof<Y>;
					static_assert(index != Meta::type_sequence<>::npos);
					
					std::clog << "\n\n" << __func__ << "\n\n\n";
					
					auto& set = *Utils::get<index>(_this->derived_sets);
					assert(ent);
					*ent = set(key, false);
				}
			};
			
			public:
			
			
			_DbSet(const Utils::tuple<DbSet<Derived>*...>& sets, const allocator_type& alloc) : _DbSet<TEntity, void, void, Allocator>(sizeof...(Derived) > 0, alloc), derived_sets(sets)
			{
				
			}
			
			virtual entity_type* find_derived_local(const Utils::type_index& tindex, const key_type& key) override
			{
				if (tindex == typeid(entity_type))
				{
					return this->get_local(key);
				}
				else if constexpr (sizeof...(Derived) > 0)
				{
					entity_type* ent = nullptr;
					bool res = Meta::type_sequence<Derived...>::template try_call_by_type_index<get_derived, decltype(this)>(tindex, this, key, &ent);
					assert(res);
					
					if (ent)
					{
						this->attach(ent);
					}
					
					return ent;
				}
				else
				{
					return nullptr;
				}
			}
			
			virtual entity_type* find_derived_local(const key_type& key) override
			{
				Utils::type_index tindex = this->get_type_for(key);
				if (tindex != typeid(void))
				{
					return find_derived_local(tindex, key);
				}
				else
				{
					return nullptr;
				}
			}
			
			
		};
		
		
		
		
		
		
		
		
		
		
		
		
		template <class TEntity, class Context, class Allocator, template <class...> class Seq, class... Derived>
		class _DbSet<TEntity, Context, Seq<Derived...>, Allocator> : public _DbSet<TEntity, void, Seq<Derived...>, Allocator>
		{
			public:
			typedef typename Utils::decay<TEntity>::type entity_type;
			typedef entity_type* value_type;
			typedef const entity_type* const_value_type;
			typedef typename Allocator::template rebind<entity_type>::other allocator_type;
			typedef typename Allocator::template rebind<ContextEntityProxy<TEntity, Context>>::other entity_allocator_type;
			typedef typename Utils::decay<Context>::type context_type;
			typedef size_t size_type;
			typedef typename model_has_key<entity_type>::key_type key_type;
			
			
			
			protected:
			context_type& context;
			
			public:
			
			_DbSet(context_type& context, const Utils::tuple<DbSet<Derived>*...>& sets, const allocator_type& alloc = allocator_type()) : _DbSet<TEntity, void, Seq<Derived...>, Allocator>(sets, alloc), context(context)
			{
				
			}
			
			virtual ~_DbSet() = default;
			
			virtual int push() override
			{
				context.start_transaction();
				
				int count = 0;
				for (auto& item : this->entity_container)
				{
					if (count % 1000 == 0)
					{
						context.end_transaction();
						context.start_transaction();
					}
					if (context.store(item))
					{
						++count;
					}
				}
				context.end_transaction();
				return count;
			}
			
			virtual int apply_changes() override
			{
				context.start_transaction();
				
				int count = 0;
				for (auto& item : this->staging_container)
				{
					/*if (count % 1000 == 0)
					{
						context.end_transaction();
						context.start_transaction();
					}*/
					if (context.store(item))
					{
						++count;
						this->attach(item);
					}
				}
				context.end_transaction();
				this->staging_container.clear();
				return count;
			}
			
			virtual bool count_remote(const key_type&) const override
			{
				// TODO
				return false;
			}
			
			virtual bool count_remote(key_type&&) const
			{
				// TODO
				return false;
			}
			
			virtual size_type size_remote() const override
			{
				return context.template size<entity_type>();
			}
			
			virtual value_type get_remote(const key_type&) override
			{
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			
			
			virtual TEntity* construct_new(const key_type& k)
			{
				if constexpr (supports_proxy<TEntity>::value)
				{
					entity_allocator_type ea(this->get_allocator());
					auto ent = ea.allocate(1);
					ea.construct(ent, context);
					ent->set_key(k);
					
					bool res = this->push_back(ent, true);
					if (!res)
					{
						return nullptr;
					}
					
					return ent;
				}
				else
				{
					auto alloc = this->get_allocator();
					auto ent = alloc.allocate(1);
					alloc.construct(ent);
					ent->*model_has_key<TEntity>::key.member = k;
					
					bool res = this->push_back(ent, true);
					if (!res)
					{
						return nullptr;
					}
					
					return ent;
				}
			}
			
			virtual Utils::type_index get_type_for(const key_type& key) const override
			{
				return context.template resolve_type<entity_type>(key);
			}
			
			
			friend Context;
		};
		
		
		
		
		
		
		
		
		
		
		
		
		
		template <class TEntity, class Context, class Allocator, template <class...> class Seq, class... Derived>
		class _AbstractDbSet<TEntity, Context, Seq<Derived...>, Allocator> : public _DbSet<TEntity, void, Seq<Derived...>, Allocator>
		{
			public:
			typedef typename Utils::decay<TEntity>::type entity_type;
			typedef entity_type* value_type;
			typedef const entity_type* const_value_type;
			typedef typename Allocator::template rebind<entity_type>::other allocator_type;
			typedef typename Allocator::template rebind<ContextEntityProxy<TEntity, Context>>::other entity_allocator_type;
			typedef typename Utils::decay<Context>::type context_type;
			typedef size_t size_type;
			typedef typename model_has_key<entity_type>::key_type key_type;
			
			
			
			protected:
			context_type& context;
			
			public:
			
			_AbstractDbSet(context_type& context, const Utils::tuple<DbSet<Derived>*...>& sets, const allocator_type& alloc = allocator_type()) : _DbSet<TEntity, void, Seq<Derived...>, Allocator>(sets, alloc), context(context)
			{
				
			}
			
			virtual ~_AbstractDbSet() = default;
			
			
			
			
			virtual int push() override
			{
				context.start_transaction();
				
				int count = 0;
				for (auto& item : this->entity_container)
				{
					if (count % 1000 == 0)
					{
						context.end_transaction();
						context.start_transaction();
					}
					if (context.store(item))
					{
						++count;
					}
				}
				context.end_transaction();
				return count;
			}
			
			virtual int apply_changes() override
			{
				context.start_transaction();
				
				int count = 0;
				for (auto& item : this->staging_container)
				{
					if (context.store(item))
					{
						++count;
						this->attach(item);
					}
				}
				context.end_transaction();
				this->staging_container.clear();
				return count;
			}
			
			virtual bool count_remote(const key_type&) const override
			{
				// TODO
				return false;
			}
			
			virtual bool count_remote(key_type&&) const
			{
				// TODO
				return false;
			}
			
			virtual size_type size_remote() const override
			{
				return context.template size<entity_type>();
			}
			
			virtual value_type get_remote(const key_type&) override
			{
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			
			
			virtual TEntity* construct_new(const key_type& k)
			{
				assert(NOT_IMPLEMENTED);
			}
			
			virtual Utils::type_index get_type_for(const key_type& key) const override
			{
				return context.template resolve_type<entity_type>(key);
			}
			
			
			friend Context;
			
			
			
			
			/*virtual bool count_remote(const key_type&) const = 0;
			virtual bool count_remote(key_type&&) const = 0;
			virtual size_type size_remote() const = 0;
			virtual value_type get_remote(const key_type&) = 0;
			virtual int push() = 0;
			virtual int apply_changes() = 0;
			virtual TEntity* construct_new(const key_type& k) = 0;*/
		};
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	template <class TEntity, class Allocator = Utils::allocator<typename Utils::decay<TEntity>::type>>
	using AbstractDbSet = DbSet<TEntity, Allocator, true>;
	
	
	template <class TEntity, class Allocator>
	class DbSet<TEntity, Allocator, true>
	{
		public:
		typedef typename Utils::decay<TEntity>::type entity_type;
		typedef entity_type* value_type;
		typedef const entity_type* const_value_type;
		typedef typename Allocator::template rebind<entity_type>::other allocator_type;
		typedef size_t size_type;
		typedef typename model_has_key<entity_type>::key_type key_type;
		
		static_assert(is_entity<entity_type>::value, "entity_type must be an entity");
		
		
		protected:
		typedef detail::_DbSet<entity_type, void, void> actor_type;
		Utils::unique_ptr<actor_type> actor;
		
		DbSet(Utils::unique_ptr<actor_type>&& ptr) : actor(Utils::forward<Utils::unique_ptr<actor_type>&&>(ptr))
		{
			
		}
		
		
		
		
		public:
		
		
		DbSet(DbSet&& other) : actor(Utils::move(other.actor))
		{
			
		}
		
		~DbSet()
		{
			actor.reset();
		}
		
		
		void attach(value_type item)
		{
			actor->attach(item);
		}
		
		bool detach(value_type item)
		{
			return actor->detach(item);
		}
		
		bool persist(value_type item)
		{
			return actor->persist(item);
		}
		
		bool lapse(value_type item)
		{
			return actor->lapse(item);
		}
		
		bool owns(value_type item) const
		{
			return actor->owns(item);
		}
		
		bool unload(value_type item)
		{
			return actor->unload(item);
		}
		
		int save()
		{
			commit();
			return push();
		}
		
		int commit()
		{
			return actor->commit();
		}
		
		int push()
		{
			return actor->push();
		}
		
		int apply_changes()
		{
			return actor->apply_changes();
		}
		
		
		
		value_type operator()(const key_type& k, bool includeRemote = true)
		{
			value_type ptr = actor->get_staging(k);
			if (!ptr)
			{
				ptr = actor->get_local(k);
			}
			if (ptr)
			{
				return ptr;
			}
			else if (actor->hasDerived)
			{
				ptr = actor->find_derived_local(k);
			}
			else if (includeRemote)
			{
				ptr = actor->get_remote(k);
			}
			
			return ptr;
		}
		
		size_type size(bool includeRemote = false) const noexcept
		{
			if (!includeRemote)
			{
				return actor->size_local() + actor->size_staging();
			}
			else
			{
				return actor->size_remote() + actor->size_staging();
			}
		}
		
		size_type count(const key_type& k, bool includeRemote = true) const
		{
			if (!actor->count_local(k))
			{
				if (!actor->count_staging(k))
				{
					if (!includeRemote || !actor->count_remote(k))
					{
						return 0;
					}
				}
			}
			return 1;
		}
			
		size_type count(const_value_type e) const
		{
			if (!actor->count_local(e))
			{
				if (!e)
				{
					return 0;
				}
				if (actor->count_staging(e))
				{
					return 1;
				}
				if (!actor->count_remote(*model_has_key<entity_type>::get_key(e)))
				{
					return 0;
				}
			}
			return 1;
		}
		
		void push_back(value_type item, bool owned = false)
		{
			actor->push_back(item, owned);
		}
		
		
		template <class Y>
		Y* create_local(const key_type& key)
		{
			static_assert(is_entity<Y>::value);
			static_assert(Utils::is_same<entity_type, Y>::value || Utils::is_base_of<entity_type, Y>::value);
			static_assert(!Utils::is_abstract<Y>::value);
			
			return actor->template create_local<Y>(key);
		}
		
		
		
		
		template <class Y = entity_type>
		Y* create(const key_type& key)
		{
			static_assert(!Utils::is_abstract<Y>::value);
			auto y = create_local<Y>(key);
			assert(y);
			
			push_back(y, true);
			
			return y;
		}
		
		template <class, class I, class A>
		friend auto create_dbset(I&, auto, const A&);
		
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	template <class TEntity, class Allocator>
	class DbSet<TEntity, Allocator, false> : public DbSet<TEntity, Allocator, true>
	{
		private:
		typedef DbSet<TEntity, Allocator, true> base_type;
		
		
		public:
		using typename base_type::entity_type;
		using typename base_type::value_type;
		using typename base_type::const_value_type;
		using typename base_type::allocator_type;
		using typename base_type::size_type;
		using typename base_type::key_type;
		
		static_assert(is_entity<entity_type>::value, "entity_type must be an entity");
		
		
		protected:
		typedef detail::_DbSet<entity_type, void, void> actor_type;
		using base_type::actor;
		
		DbSet(Utils::unique_ptr<actor_type>&& ptr) : base_type(Utils::forward<Utils::unique_ptr<actor_type>&&>(ptr))
		{
			
		}
		
		
		
		
		
		public:
		
		
		DbSet(DbSet&& other) : base_type(Utils::forward<DbSet&&>(other))
		{
			
		}
		
		~DbSet()
		{
			
		}
		
		
		
		
		
		
		template <class Y>
		Y* create_local(const key_type& key)
		{
			static_assert(is_entity<Y>::value);
			static_assert(Utils::is_same<entity_type, Y>::value || Utils::is_base_of<entity_type, Y>::value);
			static_assert(!Utils::is_abstract<Y>::value);
			
			return this->actor->template create_local<Y>(key);
		}
		
		entity_type* create_local(const key_type& key)
		{
			
			
			return this->actor->template create_local<entity_type>(key);
		}
		
		
		template <class Y = entity_type>
		Y* create(const key_type& key)
		{
			auto y = create_local<Y>(key);
			assert(y);
			
			this->push_back(y, true);
			
			return y;
		}
		
		//using base_type::create_local;
		
		/*template <class Y = entity_type>
		Y* create_local(const key_type& key)
		{
			return actor->template create_local<Y>(key);
		}*/
		
		
		template <class, class I, class A>
		friend auto create_dbset(I&, auto, const A&);
		
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	namespace detail
	{
		template <class B>
		struct is_base_of_filter
		{
			template <class Y>
			struct inner : public Utils::integral_constant<bool, Utils::is_base_of<B, Y>::value>
			{
				
			};
		};
		
		template <class... T>
		struct DbSet_Filter
		{
			typedef DbSet<T...> type;
		};
		
		template <class>
		struct dbset_extractor;
		
		template <class T, class... Y, bool B>
		struct dbset_extractor<DbSet<T, Y..., B>>
		{
			typedef T type;
		};
		
		template <class T>
		struct dbset_extractor<DbSet<T>>
		{
			typedef T type;
		};
		
		template <class TEntity, class Impl, class Types, class Allocator = Utils::allocator<typename Utils::decay<TEntity>::type>>
		struct create_dbset_struct;
		
		template <class TEntity, class Impl, template <class...> class Seq, class... _Types, class Allocator>
		struct create_dbset_struct<TEntity, Impl, Seq<_Types...>, Allocator>
		{
			typedef typename Meta::type_sequence_from<Seq<_Types...>>::type::template remove_all<TEntity> types;
			typedef detail::BuiltModel<TEntity> model;
			typedef typename model::All_Bases all_bases;
			typedef typename all_bases::template intersect<types> bases;
			
			typedef typename types::template where<is_base_of_filter<TEntity>::template inner> descendants;
			
			typedef typename descendants::template apply_transforms<DbSet_Filter> descendant_sets;
			
			typedef typename descendant_sets::template apply_transforms<Utils::add_pointer> desc_set_ptrs;
			
			
			template <class... Derived>
			static auto Create(Impl& impl, Utils::tuple<DbSet<Derived>*...> sets, const Allocator& alloc)
			{
				
				
				static_assert(Utils::is_same<Utils::tuple<DbSet<Derived>...>, typename descendant_sets::template apply_template<Utils::tuple>>::value);
				static_assert(Utils::is_same<typename desc_set_ptrs::template apply_template<Utils::tuple>, Utils::tuple<DbSet<Derived>*...>>::value);
				
				typedef typename Utils::decay<Impl>::type impl_type;
				
				typedef typename  detail::dbset_chooser<TEntity>::template inner<impl_type, descendants, Allocator>::type impl_actor_type;
				
				typedef typename Allocator::template rebind<impl_actor_type>::other impl_alloc;
				typedef Utils::allocator_traits<impl_alloc> atraits;
				
				impl_alloc ialloc(alloc);
				impl_actor_type* ptr = atraits::allocate(ialloc, 1);
				atraits::construct(ialloc, ptr, impl, sets, alloc);
				
				return Utils::unique_ptr<impl_actor_type>(ptr);
			}
		};
		
		
	}
	
	
	
	template <class TEntity, class Impl, class Allocator = Utils::allocator<typename Utils::decay<TEntity>::type>>
	auto create_dbset(Impl& impl, auto sets, const Allocator& alloc = Allocator())
	{
		typedef typename Meta::type_sequence_from<typename Utils::decay<decltype(*sets)>::type>::type::template apply_transforms<detail::dbset_extractor> all_types;
		
		typedef detail::create_dbset_struct<TEntity, Impl, all_types, Allocator> creator_struct;
		
		typedef typename all_types::template indexes<typename creator_struct::descendants> indexes;
		
		auto tup_sets = indexes::get_tuple_pointers(*sets);
		
		return DbSet<TEntity, Allocator>(creator_struct::Create(impl, tup_sets, alloc));
		
		
		
		/*typedef typename Utils::decay<Impl>::type impl_type;
		
		typedef typename  detail::dbset_chooser<TEntity, impl_type, Allocator>::type impl_actor_type;
		
		typedef typename Allocator::template rebind<impl_actor_type>::other impl_alloc;
		
		typedef Utils::allocator_traits<impl_alloc> atraits;
		
		impl_alloc ialloc(alloc);
		impl_actor_type* ptr = atraits::allocate(ialloc, 1);
		atraits::construct(ialloc, ptr, impl, alloc);
			
		return DbSet<TEntity, Allocator>(Utils::unique_ptr<impl_actor_type>(ptr));*/
	}
		
	
}
}

#endif
#endif