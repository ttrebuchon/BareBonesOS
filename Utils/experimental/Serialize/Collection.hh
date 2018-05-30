#ifndef INCLUDED_EXPERIMENTAL_SERIALIZE_COLLECTION_HH
#define INCLUDED_EXPERIMENTAL_SERIALIZE_COLLECTION_HH

#include "Context.hh"
#include "Entity.hh"
#include <Utils/detail/selectn.hh>
#include <Utils/experimental/collection_traits>
#include <Utils/unordered_set>

namespace Utils { namespace experimental {
namespace serialize
{
	
	template <class T, class Y = T>
	class CollectionBase
	{
		public:
		typedef typename Utils::decay<Y>::type* value_type;
		typedef const typename Utils::decay<Y>::type* const_value_type;
		typedef size_t size_type;
		typedef typename Utils::decay<T>::type entity_type;
		
		
		protected:
		
		
		template <class Val_t>
		class abstract_iterator
		{
			protected:
			
			public:
			
			virtual Val_t get() noexcept = 0;
			virtual void next() noexcept = 0;
			virtual abstract_iterator* copy() const noexcept = 0;
			
			virtual bool equals(const abstract_iterator*) const = 0;
		};
		
		virtual abstract_iterator<const_value_type>* _cbegin() const noexcept = 0;
		virtual abstract_iterator<const_value_type>* _cend() const noexcept = 0;
		
		
		template <class H>
		class _iterator
		{
			protected:
			abstract_iterator<H>* ptr;
			
			_iterator(abstract_iterator<H>* ptr) : ptr(ptr)
			{
				
			}
			
			public:
			
			_iterator(const _iterator& it)
			{
				if (it.ptr)
				{
					ptr = it.ptr->copy();
				}
			}
			
			_iterator(_iterator&& it)
			{
				ptr = it.ptr;
				it.ptr = nullptr;
			}
			
			~_iterator()
			{
				if (ptr)
				{
					delete ptr;
				}
			}
			
			_iterator operator++(int) noexcept
			{
				iterator it2(ptr->copy());
				ptr->next();
				return it2;
			}
			
			_iterator& operator++() noexcept
			{
				ptr->next();
				return *this;
			}
			
			H operator*() noexcept
			{
				return ptr->get();
			}
			
			bool operator==(const _iterator& i) const
			{
				return ptr->equals(i.ptr);
			}
			
			bool operator!=(const _iterator& i) const
			{
				return !ptr->equals(i.ptr);
			}
			
			template <class, class>
			friend class CollectionBase;
		};
		
		
		
		public:
		typedef _iterator<value_type> iterator;
		typedef _iterator<const_value_type> const_iterator;
		
		virtual size_type size() const noexcept = 0;
		
		virtual void add(value_type) = 0;
		
		
		const_iterator begin() const noexcept
		{
			return const_iterator(_cbegin());
		}
		
		const_iterator end() const noexcept
		{
			return const_iterator(_cend());
		}
		
		const_iterator cbegin() const noexcept
		{
			return const_iterator(_cbegin());
		}
		
		const_iterator cend() const noexcept
		{
			return const_iterator(_cend());
		}
	};
	
	
	
	
	
	template <class T, template <class...> class Container, class Item = T, class Allocator = allocator<_Entity<T>>, class Select_t = Utils::SelectIdentity>
	class StorageCollection : public CollectionBase<T>
	{
		protected:
		typedef CollectionBase<T, Item> base_type;
		
		
		
		public:
		using typename base_type::value_type;
		using typename base_type::const_value_type;
		using typename base_type::entity_type;
		using typename base_type::size_type;
		typedef Allocator allocator_type;
		typedef Container<value_type, allocator_type> container_type;
		typedef Select_t select_type;
		typedef container_traits<container_type> traits_type;
		
		protected:
		container_type under;
		
		template <class H>
		using abstract_iterator = typename base_type::template abstract_iterator<H>;
		
		class iter : public abstract_iterator<value_type>
		{
			protected:
			typename container_type::iterator it;
			
			
			public:
			iter(typename container_type::iterator it) : it(it)
			{}
			
			
			virtual value_type get() noexcept override
			{
				return *it;
			}
			
			virtual void next() noexcept override
			{
				++it;
			}
			
			virtual iter* copy() const noexcept override
			{
				return new iter(it);
			}
			
			virtual bool equals(const abstract_iterator<value_type>* rhs) const override
			{
				auto dyn = dynamic_cast<const iter*>(rhs);
				if (dyn)
				{
					return it == dyn->it;
				}
				return false;
			}
		};
		
		
		class const_iter : public abstract_iterator<const_value_type>
		{
			protected:
			typename container_type::const_iterator it;
			
			
			public:
			const_iter(typename container_type::const_iterator it) : it(it)
			{}
			
			
			virtual const_value_type get() noexcept override
			{
				return *it;
			}
			
			virtual void next() noexcept override
			{
				++it;
			}
			
			virtual const_iter* copy() const noexcept override
			{
				return new const_iter(it);
			}
			
			virtual bool equals(const abstract_iterator<const_value_type>* rhs) const override
			{
				auto dyn = dynamic_cast<const const_iter*>(rhs);
				if (dyn)
				{
					return it == dyn->it;
				}
				return false;
			}
		};
		
		virtual const_iter* _cbegin() const noexcept override
		{
			return new const_iter(under.cbegin());
		}
		virtual const_iter* _cend() const noexcept override
		{
			return new const_iter(under.cend());
		}
		
		
		public:
		
		virtual size_type size() const noexcept override
		{
			return traits_type::size(under);
			return under.size();
		}
		
		virtual void add(value_type item) override
		{
			//under.push_back(item);
			traits_type::insert(under, item);
		}
	};
	
	
	template <class T, class Alloc = allocator<_Entity<T>>>
	class HashedVectorCollection : public CollectionBase<T>
	{
		protected:
		typedef CollectionBase<T> base_type;
		
		
		
		public:
		using typename base_type::value_type;
		using typename base_type::const_value_type;
		using typename base_type::entity_type;
		using typename base_type::size_type;
		typedef Alloc allocator_type;
		
		protected:
		//unordered_set<value_type, allo
		
		public:
	};
	
	
	
	
	
	
	
	template <class T, class H = T>
	class Collection
	{
		public:
		typedef typename Utils::decay<H>::type value_type;
		typedef typename Utils::decay<T>::type entity_type;
		
		protected:
		Context& _context;
		CollectionBase<T, H>* impl;
		
		Collection(Context& c, CollectionBase<T, H>* imp) : _context(c), impl(imp)
		{
			
		}
		
		
		public:
		Collection(Context& c) : _context(c), impl(nullptr)
		{
			impl = new StorageCollection<T, Utils::list>;
		}
		
		template <class Y, class... Args>
		Y* create(Args... args)
		{
			static_assert(Utils::is_base_of<T, Y>::value);
			static_assert(Utils::is_polymorphic<T>::value);
			
			Y* e;
			
			
			auto model = _context.types().get_model<Y>();
			if (!model || !model->is_entity())
			{
				auto model2 = _context.types().get_model<T>();
				assert(model2);
				assert(model2->is_entity());
				typedef _Entity<Y, T> E2;
				e = new E2(_context, model2, Utils::forward<Args>(args)...);
			}
			else
			{
				typedef _Entity<Y> E;
				e = new E(_context, model, Utils::forward<Args>(args)...);
			}
			
			__try
			{
				impl->add(e);
			}
			__catch(...)
			{
				if (e)
				{
					delete e;
				}
				__throw_exception_again;
			}
			
			
			return e;
		}
		
		bool save()
		{
			map<const reflect::Model*, Utils::list<ValueSet*>> values;
			//list<ValueSet*> values;
			const Entity* dyn;
			for (const auto& item : *impl)
			{
				dyn = dynamic_cast<const Entity*>(item);
				if (!dyn)
				{
					assert(NOT_IMPLEMENTED);
				}
				auto model = dyn->model();
				assert(model);
				values[model].push_back(dyn->get_values());
				//values.push_back(dyn->get_values());
			}
			
			return _context._save(move(values));
		}
	};
	
	
	template <class Key, class T>
	class UnorderedSetCollection : public Collection<T>
	{
		protected:
		typedef Collection<T> base_type;
		
		public:
		using typename base_type::value_type;
		using typename base_type::entity_type;
		
		UnorderedSetCollection(Context& c) : base_type(c, new StorageCollection<T, unordered_set>())
		{
			
		}
	};
	
	
	template <class Key, class T>
	class MapCollection : public Collection<T, Utils::pair<Key, T*>>
	{
		protected:
		typedef Collection<T, pair<Key, T*>> base_type;
		
		public:
		using typename base_type::value_type;
		using typename base_type::entity_type;
		
		MapCollection(Context& c) : base_type(c, new StorageCollection<T, unordered_map>())
		{
			
		}
		
		
	};
	
	
	
}
}
}


#endif