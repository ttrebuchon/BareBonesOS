#ifndef INCLUDED_EXPERIMENTAL_REFLECT_TYPEMODEL_HPP
#define INCLUDED_EXPERIMENTAL_REFLECT_TYPEMODEL_HPP

#include "TypeModel.hh"
#include <Utils/experimental/Serialize/Value.hh>

namespace Utils { namespace experimental {
namespace reflect
{
	template <class T>
	bool TypedModel<T>::is_satisfied(const TypeSystem& ts) const noexcept
	{
		for (const auto& p : props)
		{
			if (!p.second->is_fundamental())
			{
				
				if (!ts.has_model_for(p.second->property_typeinfo()->bottom()->info()))
				{
					return false;
				}
			}
		}
		
		for (auto& b : _bases)
		{
			if (b.linked)
			{
				
			}
			else if (!ts.has_model_for(*b.info))
			{
				return false;
			}
		}
		
		return true;
	}
	
	
	template <class T>
	serialize::ValueSet* TypedModel<T>::get_value_set(const void* obj) const
	{
		return serialize::ObjectValueSet<T>::Gather(this, (const T*)obj);
	}
	
	template <class T>
	bool TypedModel<T>::restore_value_set(const serialize::ValueSet* v, void* obj) const
	{
		
		assert(!this->get_type()->is_const());
		assert(dynamic_cast<const serialize::ObjectValueSet<type>*>(v));
		dynamic_cast<const serialize::ObjectValueSet<type>*>(v)->restore(this, (type*)obj);
		return true;
	}
	
	template <class T>
	bool TypedModel<T>::link(TypeSystem& ts)
	{
		for (auto& base : _bases)
		{
			if (!base.linked)
			{
				auto mod = ts.get_model(*base.info);
				if (!mod)
				{
					return false;
				}
				
				base = BaseType(mod, base.off);
			}
		}
		
		
		return true;
	}
}
}
}

#endif