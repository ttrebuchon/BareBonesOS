#ifndef INCLUDED_EXPERIMENTAL_SERIALIZE_VALUE_HH
#define INCLUDED_EXPERIMENTAL_SERIALIZE_VALUE_HH

#include <Common.h>
#include <Utils/experimental/reflect>

namespace Utils { namespace experimental {
namespace serialize
{
	class Value
	{
		public:
		
		protected:
		
		
		public:
		
		virtual reflect::FundamentalType fundamental() const noexcept = 0;
		
		virtual const std::type_info& info() const noexcept = 0;
		
		virtual const std::type_info& value_typeinfo() const noexcept = 0;
		
		virtual bool is_pointer() const noexcept = 0;
		virtual bool delete_pointer() = 0;
	};
	
	template <class T>
	class TypedValue : public Value
	{
		public:
		typedef typename decay<T>::type decayed;
		typedef typename decay<T>::type type;
		
		protected:
		
		public:
		virtual type& value() noexcept = 0;
		virtual const type& value() const noexcept = 0;
		
		virtual reflect::FundamentalType fundamental() const noexcept
		{
			return reflect::Fundamental<type>::value;
		}
		
		virtual const std::type_info& info() const noexcept
		{
			return typeid(type);
		}
		
		virtual const std::type_info& value_typeinfo() const noexcept
		{
			return typeid(value());
		}
		
		virtual bool is_pointer() const noexcept
		{
			return false;
		}
		
		virtual bool delete_pointer() override
		{
			return false;
		}
	};
	
	
	template <class T>
	class TypedValue<T*> : public Value
	{
		public:
		typedef typename decay<T>::type decayed;
		typedef typename decay<T*>::type type;
		
		protected:
		
		
		public:
		virtual type& value() noexcept = 0;
		virtual const type& value() const noexcept = 0;
		
		
		virtual reflect::FundamentalType fundamental() const noexcept
		{
			return reflect::Fundamental<decayed>::value;
		}
		
		virtual const std::type_info& info() const noexcept
		{
			return typeid(decayed);
		}
		
		virtual const std::type_info& value_typeinfo() const noexcept
		{
			return typeid(value());
		}
		
		virtual bool is_pointer() const noexcept
		{
			return true;
		}
		
		virtual bool delete_pointer() override
		{
			auto v = value();
			if (v != nullptr)
			{
				delete v;
				value() = nullptr;
				return true;
			}
			return false;
		}
	};
	
	
	
	
	
	template <class Obj, class F>
	class PropertyValue : public TypedValue<F>
	{
		protected:
		typedef TypedValue<F> base_type;
		
		
		public:
		typedef typename base_type::decayed decayed;
		typedef typename base_type::type type;
		
		
		protected:
		Obj* obj;
		reflect::MemberProperty<Obj, type>* prop;
		
		
		
		public:
		
		
		
		virtual type& value() noexcept override
		{
			return prop->value(*obj);
		}
		
		virtual const type& value() const noexcept override
		{
			return prop->value(*obj);
		}
		
		
		
	};
	
	
	template <class T>
	class BasicValue : public TypedValue<T>
	{
		protected:
		typedef TypedValue<T> base_type;
		
		
		public:
		typedef typename base_type::decayed decayed;
		typedef typename base_type::type type;
		
		
		protected:
		type _value;
		
		public:
		BasicValue(const T& t) : base_type(), _value(t)
		{}
		
		BasicValue(T&& t) : base_type(), _value(forward<T&&>(t))
		{}
		
		template <class... Args>
		BasicValue(Args... args) : base_type(), _value(forward<Args>(args)...)
		{}
		
		
		virtual type& value() noexcept override
		{
			return _value;
		}
		
		virtual const type& value() const noexcept override
		{
			return _value;
		}
	};
	
	
	
	
	
	
	
	
	class ValueSet
	{
		public:
		
		protected:
		
		unordered_map<string, Value*> _values;
		unordered_map<string, ValueSet*> _bases;
		const reflect::Model* _model;
		
		public:
		ValueSet(const reflect::Model* model) : _model(model)
		{
			
		}
		
		virtual ~ValueSet() = default;
		
		static ValueSet* Gather(const reflect::Model* m, const void* obj)
		{
			return m->get_value_set(obj);
		}
		
		void restore(const reflect::Model* m, void* obj) const
		{
			m->restore_value_set(this, obj);
		}
		
		virtual const reflect::Type* type_object() const = 0;
		
		Value* value(const string& str, bool lookThroughBases = false)
		{
			if (_values.count(str) > 0)
			{
				return _values.at(str);
			}
			
			if (lookThroughBases)
			{
				for (auto& b : _bases)
				{
					auto v = b.second->value(str, true);
					if (v)
					{
						return v;
					}
				}
			}
			
			return nullptr;
		}
		
		ValueSet* base(const string& name)
		{
			if (_bases.count(name))
			{
				return _bases.at(name);
			}
			else
			{
				return nullptr;
			}
		}
		
		const reflect::Model* model() const noexcept
		{
			return _model;
		}
	};
	
	
	
	template <class T>
	class ObjectValueSet : public ValueSet
	{
		public:
		typedef T type;
		
		protected:
		
		
		public:
		ObjectValueSet(const reflect::Model* model) : ValueSet(model)
		{
			
		}
		
		
		static ObjectValueSet* Gather(const reflect::TypedModel<type>* m, const type* obj)
		{
			auto set = new ObjectValueSet(m);
			set->_values.reserve(m->size());
			
			for (auto& p : m->properties())
			{
				auto v = p.second->to_value(obj);
				set->_values[p.first] = v;
			}
			
			for (auto& b : m->bases())
			{
				auto v = ValueSet::Gather(b.model, (void*)((const char*)obj + b.off));
				set->_bases[b.model->name()] = v;
			}
			
			return set;
		}
		
		
		template <class Y>
		static ObjectValueSet* Gather(const reflect::TypedModel<type>* m, const Y* obj)
		{
			static_assert(Utils::is_polymorphic<Y>::value);
			auto ptr = dynamic_cast<const type*>(obj);
			assert(ptr);
			return ObjectValueSet::Gather(m, ptr);
		}
		
		
		
		void restore(const reflect::TypedModel<type>* m, type* obj) const
		{
			for (auto& b : m->bases())
			{
				auto baseValues = _bases.at(b.model->name());
				baseValues->restore(b.model, (void*)((char*)obj + b.off));
			}
			
			for (auto& p : m->properties())
			{
				auto v = _values.at(p.first);
				assert(v);
				if (v->fundamental() != reflect::FundamentalType::Not)
				{
					p.second->set(obj, v);
				}
				else
				{
					assert(NOT_IMPLEMENTED);
				}
			}
		}
		
		virtual const reflect::Type* type_object() const override
		{
			return reflect::Type::Get<type>();
		}
		
		
		template <class Y>
		class PointerValue : public TypedValue<Y>
		{
			
		};
	};
}
}
}

#endif