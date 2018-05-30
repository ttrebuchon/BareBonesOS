#ifndef INCLUDED_EXPERIMENTAL_REFLECT_TYPEMODEL_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_TYPEMODEL_HH

#include <Common.h>
#include "Primitive.hh"
#include "Properties.hh"
#include "BaseType.hh"
#include "Type.hh"
#include <Utils/unordered_map>
#include <Utils/type_traits>
#include <Utils/list>

namespace Utils { namespace experimental {
namespace serialize
{
	class ValueSet;
}
}
}

namespace Utils { namespace experimental {
namespace reflect
{
	class TypeSystem;
	class GenericProperty;
	
	
	
	class Model
	{
		protected:
		list<BaseType> _bases;
		
		template <class Property_t>
		class abstract_iterator
		{
			protected:
			
			public:
			
			virtual Property_t get() noexcept = 0;
			virtual void next() noexcept = 0;
			virtual abstract_iterator* copy() const noexcept = 0;
			
			virtual bool equals(const abstract_iterator*) const = 0;
		};
		
		virtual abstract_iterator<const GenericProperty*>* _cbegin() const noexcept = 0;
		virtual abstract_iterator<const GenericProperty*>* _cend() const noexcept = 0;
		
		public:
		class iterator
		{
			protected:
			abstract_iterator<const GenericProperty*>* ptr;
			
			iterator(abstract_iterator<const GenericProperty*>* ptr) : ptr(ptr)
			{
				
			}
			
			public:
			
			iterator(const iterator& it)
			{
				if (it.ptr)
				{
					ptr = it.ptr->copy();
				}
			}
			
			iterator(iterator&& it)
			{
				ptr = it.ptr;
				it.ptr = nullptr;
			}
			
			~iterator()
			{
				if (ptr)
				{
					delete ptr;
				}
			}
			
			iterator operator++(int) noexcept
			{
				iterator it2(ptr->copy());
				ptr->next();
				return it2;
			}
			
			iterator& operator++() noexcept
			{
				ptr->next();
				return *this;
			}
			
			const GenericProperty* operator*() const noexcept
			{
				return ptr->get();
			}
			
			bool operator==(const iterator& i) const
			{
				return ptr->equals(i.ptr);
			}
			
			bool operator!=(const iterator& i) const
			{
				return !ptr->equals(i.ptr);
			}
			
			friend class Model;
		};
		
		
		virtual const std::type_info& object_typeinfo() const noexcept = 0;
		virtual size_t count() const noexcept = 0;
		
		virtual bool is_satisfied(const TypeSystem&) const noexcept = 0;
		
		virtual bool is_entity() const noexcept = 0;
		virtual bool is_class() const noexcept = 0;
		virtual bool is_polymorphic() const noexcept = 0;
		virtual bool is_abstract() const noexcept = 0;
		virtual const GenericProperty* get_key(const Model** = nullptr) const noexcept = 0;
		virtual GenericProperty* get_key(const Model** = nullptr) noexcept = 0;
		virtual const Type* get_type() const = 0;
		virtual const string& name() const noexcept = 0;
		virtual serialize::ValueSet* get_value_set(const void*) const = 0;
		virtual bool restore_value_set(const serialize::ValueSet*, void*) const = 0;
		virtual bool link(TypeSystem&) = 0;
		//virtual void* offset_object(const Model*, void* obj) const = 0;
		
		
		const list<BaseType>& bases() const noexcept
		{
			return _bases;
		}
		
		
		
		iterator begin() const noexcept
		{
			return iterator(_cbegin());
		}
		
		iterator end() const noexcept
		{
			return iterator(_cend());
		}
		
	};
	
	template <class T>
	class TypedModel : public Model
	{
		public:
		typedef typename Utils::decay<T>::type type;
		
		
		protected:
		unordered_map<string, Property<type>*> props;
		bool _entity = true;
		string _key_name;
		string _name;
		const TypedType<type>* _type;
		
		
		class iter : public abstract_iterator<const GenericProperty*>
		{
			protected:
			typename unordered_map<string, Property<type>*>::const_iterator it;
			
			
			public:
			iter(typename unordered_map<string, Property<type>*>::const_iterator it) : it(it)
			{}
			
			
			virtual const Property<T>* get() noexcept override
			{
				return (*it).second;
			}
			
			virtual void next() noexcept override
			{
				++it;
			}
			
			virtual iter* copy() const noexcept override
			{
				return new iter(it);
			}
			
			virtual bool equals(const abstract_iterator<const GenericProperty*>* rhs) const override
			{
				auto dyn = dynamic_cast<const iter*>(rhs);
				if (dyn)
				{
					return it == dyn->it;
				}
				return false;
			}
		};
		
		
		virtual iter* _cbegin() const noexcept override
		{
			return new iter(props.cbegin());
		}
		
		virtual iter* _cend() const noexcept override
		{
			return new iter(props.cend());
		}
		
		public:
		TypedModel(unordered_map<string, Property<type>*>&& props, bool entity, list<BaseType>&& bases, const string& key_name, const string& name) :
		
		props(Utils::forward<unordered_map<string, Property<type>*>&&>(props)),
		_entity(entity),
		_key_name(key_name),
		_name(name),
		_type(nullptr)
		
		{
			_bases = Utils::forward<list<BaseType>&&>(bases);
			_type = Type::Get<type>();
		}
		
		virtual const std::type_info& object_typeinfo() const noexcept override
		{
			return typeid(type);
		}
		
		bool is_entity() const noexcept override
		{
			return _entity;
		}
		
		virtual bool is_class() const noexcept override
		{
			return Utils::is_class<type>::value;
		}
		
		virtual bool is_polymorphic() const noexcept override
		{
			return Utils::is_polymorphic<type>::value;
		}
		
		virtual bool is_abstract() const noexcept override
		{
			return Utils::is_abstract<type>::value;
		}
		
		virtual const /*Property<T>**/GenericProperty* get_key(const Model** m = nullptr) const noexcept override
		{
			if (is_entity())
			{
				if (props.count(_key_name))
				{
					return props.at(_key_name);
				}
				for (auto base : _bases)
				{
					if (!base.linked)
					{
						continue;
					}
					assert(base.model);
					auto k = base.model->get_key();
					if (k)
					{
						if (m)
						{
							*m = base.model;
						}
						return k;
					}
				}
			}
			return nullptr;
		}
		
		virtual GenericProperty* get_key(const Model** = nullptr) noexcept override
		{
			if (is_entity())
			{
				if (props.count(_key_name))
				{
					return props.at(_key_name);
				}
			}
			return nullptr;
		}
		
		virtual size_t count() const noexcept
		{
			return props.size();
		}
		
		size_t size() const noexcept
		{
			return props.size();
		}
		
		virtual bool is_satisfied(const TypeSystem&) const noexcept override;
		
		virtual const Type* get_type() const override
		{
			return _type;
		}
		
		const unordered_map<string, Property<type>*>& properties() const noexcept
		{
			return props;
		}
		
		virtual const string& name() const noexcept override
		{
			return _name;
		}
		
		
		virtual serialize::ValueSet* get_value_set(const void*) const override;
		virtual bool restore_value_set(const serialize::ValueSet*, void*) const override;
		virtual bool link(TypeSystem&) override;
	};
	
	
	
}
}
}

#endif