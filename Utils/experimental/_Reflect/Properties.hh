#ifndef INCLUDED_EXPERIMENTAL_REFLECT_PROPERTIES_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_PROPERTIES_HH

#include <Common.h>
#include <Utils/utility>
#include <Utils/functional>
#include <Utils/string>
#include <Utils/unordered_map>
#include "Primitive.hh"
#include <Utils/type_traits>
#include <Utils/list>
#include "Type.hh"
#include "helpers"

namespace Utils { namespace experimental {
namespace serialize
{
	class Value;
	template <class>
	class TypedValue;
	
	
	template <class>
	class Prong;
}
}
}

namespace Utils { namespace experimental {
namespace reflect
{
	template <class>
	class TypedModel;
	
	
	class GenericProperty
	{
		public:
		
		protected:
		
		public:
		GenericProperty() = default;
		
		virtual const Type* object_typeinfo() const = 0;
		virtual const Type* property_typeinfo() const = 0;
		
		virtual FundamentalType fundamental() const noexcept = 0;
		bool is_fundamental() const noexcept
		{
			return fundamental() != FundamentalType::Not;
		}
		
		bool is_pointer() const noexcept
		{
			return property_typeinfo()->is_pointer();
		}
		
		bool is_const() const noexcept
		{
			return property_typeinfo()->is_const();
		}
		
		bool is_volatile() const noexcept
		{
			return property_typeinfo()->is_volatile();
		}
		
		bool is_reference() const noexcept
		{
			return property_typeinfo()->is_reference();
		}
		
		virtual bool is_field() const noexcept = 0;
		virtual const string& name() const noexcept = 0;
	};
	
	
	
	template <class T, class F>
	class FieldBasedProperty : virtual public GenericProperty
	{
		public:
		typedef typename decay<F>::type property_type;
		
		protected:
		
		public:
		
		virtual property_type& value(T& t) const noexcept = 0;
		virtual bool is_field() const noexcept
		{
			return true;
		}
	};
	
	
	template <class T, class F>
	class FieldBasedProperty<T, const F> : virtual public GenericProperty
	{
		public:
		typedef typename decay<const F>::type property_type;
		
		protected:
		
		public:
		
		virtual const property_type& value(const T& t) const noexcept = 0;
		virtual bool is_field() const noexcept
		{
			return true;
		}
	};
	
	
	
	template <class T>
	class Property : virtual public GenericProperty
	{
		public:
		typedef typename decay<T>::type object_type;
		
		
		protected:
		typedef GenericProperty base_type;
		
		public:
		Property() : base_type()
		{
			
		}
		
		
		virtual const Type* object_typeinfo() const
		{
			return Type::Get<object_type>();
		}
		
		virtual serialize::Value* to_value(const object_type*) const = 0;
		virtual bool set(object_type*, const serialize::Value*) const = 0;
		
		virtual serialize::Prong<void>* make_prong(const TypedModel<object_type>*, object_type* obj) const;
	};
	
	
	
	
	
	
	template <class T, class F>
	class TypedProperty : virtual public Property<T>
	{
		public:
		typedef typename decay<F>::type property_type;
		typedef F member_type;
		
		typedef typename Property<T>::object_type object_type;
		
		protected:
		typedef Property<T> base_type;
		
		public:
		TypedProperty() : base_type()
		{
			
		}
		
		
		virtual const Type* property_typeinfo() const
		{
			return Type::Get<property_type>();
		}
		
		virtual FundamentalType fundamental() const noexcept override
		{
			return Fundamental<property_type>::value;
		}
		
		
		virtual void get(const T& t, member_type&) const = 0;
		virtual void set(T& t, const member_type&) const = 0;
		
		virtual serialize::Value* to_value(const object_type*) const override;
		virtual bool set(object_type* obj, const serialize::Value* v) const override
		{
			auto casted = dynamic_cast<const serialize::TypedValue<property_type>*>(v);
			if (casted)
			{
				this->set(*obj, casted->value());
				return true;
			}
			
			assert(NOT_IMPLEMENTED);
		}
		
		virtual serialize::Prong<void>* make_prong(const TypedModel<object_type>*, object_type* obj) const override;
	};
	
	
	
	
	
	
	
	
	template <class T, class F>
	class TypedProperty<T, const F> : virtual public Property<T>
	{
		public:
		typedef typename decay<const F>::type property_type;
		typedef const F member_type;
		
		typedef typename Property<T>::object_type object_type;
		
		protected:
		typedef Property<T> base_type;
		
		public:
		TypedProperty() : base_type()
		{
			
		}
		
		
		virtual const Type* property_typeinfo() const
		{
			return Type::Get<member_type>();
		}
		
		virtual FundamentalType fundamental() const noexcept override
		{
			return Fundamental<property_type>::value;
		}
		
		
		virtual void get(const T& t, F&) const = 0;
		
		virtual serialize::Value* to_value(const object_type*) const override;
		virtual bool set(object_type*, const serialize::Value*) const
		{
			return false;
		}
	};
	
	
	
	
	
	
	
	
	template <class T, class F>
	class MemberProperty : public TypedProperty<T, F>, public FieldBasedProperty<T, F>
	{
		protected:
		typedef TypedProperty<T, F> base_type;
		
		public:
		
		typedef F T::*value_type;
		
		typedef typename base_type::property_type property_type;
		typedef typename base_type::object_type object_type;
		
		protected:
		value_type ptr;
		string _name;
		
		public:
		MemberProperty(const string& name, value_type p) : base_type(), _name(name), ptr(p)
		{
			
		}
		
		//using base_type::object_typeinfo;
		
		virtual void get(const T& t, property_type& v) const noexcept
		{
			v = t.*ptr;
		}
		
		virtual void set(T& t, const property_type& v) const noexcept
		{
			t.*ptr = v;
		}
		
		virtual property_type& value(T& t) const noexcept override
		{
			return t.*ptr;
		}
		
		virtual const string& name() const noexcept override
		{
			return _name;
		}
	};
	
	
	
	
	
	
	
	template <class T, class F>
	class MemberProperty<T, const F> : public TypedProperty<T, const F>, public FieldBasedProperty<T, const F>
	{
		protected:
		typedef TypedProperty<T, const F> base_type;
		
		public:
		
		typedef const F T::*value_type;
		string _name;
		
		typedef typename base_type::property_type property_type;
		typedef typename base_type::object_type object_type;
		
		protected:
		value_type ptr;
		
		public:
		MemberProperty(const string& name, value_type p) : base_type(), _name(name), ptr(p)
		{
			
		}
		
		virtual void get(const T& t, F& v) const noexcept
		{
			v = t.*ptr;
		}
		
		virtual const property_type& value(const T& t) const noexcept override
		{
			return t.*ptr;
		}
		
		virtual const string& name() const noexcept override
		{
			return _name;
		}
	};
	
	
	
	
	
	
	
	
	
	
	
	
	template <class T, class F>
	class LambdaProperty : public TypedProperty<T, F>
	{
		protected:
		typedef TypedProperty<T, F> base_type;
		
		public:
		
		typedef function<void(T&, const F&)> setter_type;
		typedef function<F(const T&)> getter_type;
		
		typedef typename base_type::property_type property_type;
		typedef typename base_type::object_type object_type;
		
		protected:
		getter_type getter;
		setter_type setter;
		
		public:
		LambdaProperty(const getter_type& g, const setter_type& s) : base_type(), getter(g), setter(s)
		{
			
		}
		
		LambdaProperty(const setter_type& s, const getter_type& g) : base_type(), getter(g), setter(s)
		{
			
		}
		
		virtual void get(const T& t, property_type& v) const noexcept
		{
			v = getter(t);
		}
		
		virtual void set(T& t, const property_type& v) const noexcept
		{
			setter(t, v);
		}
		
		virtual bool is_field() const noexcept override
		{
			return false;
		}
	};
	
	
	
	
	namespace detail
	{
		
	}
	
	
	
	
	auto getProperty(const string& name, auto field)
	{
		typedef detail::field_decomp<decltype(field)> dec;
		typedef typename dec::obj T;
		typedef typename dec::value F;
		return new MemberProperty<T, F>(name, field);
	}
	
	
	
	
	
	class TypeSystem;
	
	class GenericPropertyModel
	{
		public:
		
		virtual const std::type_info& object_typeinfo() const noexcept = 0;
		virtual size_t count() const noexcept = 0;
		
		virtual bool is_satisfied(const TypeSystem&) const noexcept = 0;
		
		virtual bool is_entity() const noexcept = 0;
		virtual bool is_class() const noexcept = 0;
		virtual bool is_polymorphic() const noexcept = 0;
		virtual bool is_abstract() const noexcept = 0;
		virtual const GenericProperty* get_key() const noexcept = 0;
		virtual GenericProperty* get_key() noexcept = 0;
		
	};
	
	
	
	
	
	template <class T>
	class PropertyModel : public GenericPropertyModel
	{
		public:
		typedef typename Utils::decay<T>::type type;
		
		
		protected:
		unordered_map<string, Property<type>*> props;
		bool _entity = true;
		list<const type_info*> bases;
		string _key_name;
		
		public:
		
		
		/*template <class F>
		PropertyModel& operator&(typename MemberProperty<T, F>::value_type p)
		{
			props.push_back(new 
		}*/
		
		/*template <class F>
		PropertyModel& add(const string& name, typename MemberProperty<T, F>::value_type p)
		{
			assert(props.count(name) == 0);
			props[name] = new MemberProperty<T, F>(p);
			return *this;
		}*/
		
		virtual const std::type_info& object_typeinfo() const noexcept override
		{
			return typeid(type);
		}
		
		PropertyModel& add(const string& name, auto field)
		{
			typedef detail::field_decomp<decltype(field)> dec;
			typedef typename dec::value F;
			assert(props.count(name) == 0);
			props[name] = new MemberProperty<type, F>(field);
			return *this;
		}
		
		PropertyModel& key(const string& name)
		{
			_key_name = name;
			return *this;
		}
		
		template <class... Args>
		PropertyModel& key(const string& name, Args... args)
		{
			add(name, forward<Args>(args)...);
			return key(name);
		}
		
		bool is_entity() const noexcept override
		{
			return _entity;
		}
		
		PropertyModel& entity(bool value = true) noexcept
		{
			_entity = value;
			return *this;
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
		
		virtual const Property<T>* get_key() const noexcept override
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
		
		virtual GenericProperty* get_key() noexcept override
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
		
		
		
		auto begin()
		{
			return props.begin();
		}
		
		auto end()
		{
			return props.end();
		}
		
		auto begin() const
		{
			return props.begin();
		}
		
		auto end() const
		{
			return props.end();
		}
		
		auto cbegin() const
		{
			return props.cbegin();
		}
		
		auto cend() const
		{
			return props.cend();
		}
	};
}
}
}


#endif