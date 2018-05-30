#ifndef INCLUDED_EXPERIMENTAL_REFLECT_MODELBUILDER_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_MODELBUILDER_HH

#include <Common.h>
#include "Primitive.hh"
#include "BaseType.hh"

namespace Utils { namespace experimental {
namespace reflect
{
	class Model;
	template <class>
	class TypedModel;
	
	template <class>
	class Property;
	
	template <class T>
	class ModelBuilder
	{
		public:
		typedef typename Utils::decay<T>::type type;
		
		
		protected:
		unordered_map<string, Property<type>*> props;
		bool _entity = true;
		list<BaseType> bases;
		string _key_name;
		string _name;
		
		
		virtual TypedModel<T>* build();
		
		public:
		
		ModelBuilder& add(const string& name, auto field);
		
		ModelBuilder& key(const string& name)
		{
			_key_name = name;
			return *this;
		}
		
		template <class... Args>
		ModelBuilder& key(const string& name, Args... args)
		{
			add(name, forward<Args>(args)...);
			return key(name);
		}
		
		ModelBuilder& entity(bool value = true) noexcept
		{
			_entity = value;
			return *this;
		}
		
		ModelBuilder& name(const string& name)
		{
			_name = name;
			return *this;
		}
		
		template <class Y>
		ModelBuilder& inherits_from()
		{
			bases.emplace_back(&typeid(Y), (char*)(Y*)(T*)(void*)0x1000 - (char*)(T*)(void*)0x1000);
			return *this;
		}
		
		template <class K>
		ModelBuilder& auto_key()
		{
			if (_auto_key)
			{
				delete _auto_key;
			}
			_auto_key = new auto_key_t<K>;
		}
		
		
		
		protected:
		struct auto_key_base
		{
			virtual TypedModel<T>* build(ModelBuilder<T>*) = 0;
		};
		
		template <class Y>
		struct auto_key_t : public auto_key_base
		{
			typedef typename decay<Y>::type type;
			
			virtual ~auto_key_t() = default;
			virtual TypedModel<T>* build(ModelBuilder<T>*) override;
		};
		
		auto_key_base* _auto_key = nullptr;
		
		
		
		friend class TypeSystem;
	};
	
	template <class T>
	using ModelBuilder_fn = void(*)(ModelBuilder<T>&);
}
}
}

#endif