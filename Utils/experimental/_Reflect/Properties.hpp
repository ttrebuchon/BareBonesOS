#ifndef INCLUDED_EXPERIMENTAL_REFLECT_PROPERTIES_HPP
#define INCLUDED_EXPERIMENTAL_REFLECT_PROPERTIES_HPP
#include "Properties.hh"
#include "TypeSystem.hh"
#include <Utils/experimental/Serialize/serialize>

namespace Utils { namespace experimental {
namespace reflect
{
	
	template <class T>
	bool PropertyModel<T>::is_satisfied(const TypeSystem& ts) const noexcept
	{
		for (auto p : props)
		{
			if (!p.second->is_fundamental())
			{
				if (!ts.has_model_for(p.second->property_typeinfo()))
				{
					return false;
				}
			}
		}
		
		return true;
	}
	
	
	template <class T>
	auto Property<T>::make_prong(const TypedModel<object_type>* model, object_type* obj) const -> serialize::Prong<>*
	{
		return serialize::ProngFactory<object_type>::Call(this, obj, model);
	}
	
	
	template <class T, class F>
	serialize::Value* TypedProperty<T, F>::to_value(const object_type* o) const
	{
		member_type tmp;
		get(*o, tmp);
		return new serialize::BasicValue<member_type>(tmp);
	}
	
	template <class T, class F>
	serialize::Value* TypedProperty<T, const F>::to_value(const object_type* o) const
	{
		property_type tmp;
		get(*o, tmp);
		return new serialize::BasicValue<member_type>(tmp);
	}
	
	template <class T, class F>
	auto TypedProperty<T, F>::make_prong(const TypedModel<object_type>* model, object_type* obj) const -> serialize::Prong<>*
	{
		return serialize::ProngFactory<object_type, F>::Call(this, obj, model);
	}
}
}
}

#endif