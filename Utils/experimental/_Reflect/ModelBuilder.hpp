#ifndef INCLUDED_EXPERIMENTAL_REFLECT_MODELBUILDER_HPP
#define INCLUDED_EXPERIMENTAL_REFLECT_MODELBUILDER_HPP

#include "ModelBuilder.hh"
#include "Properties.hh"
#include "helpers"

namespace Utils { namespace experimental {
namespace reflect
{
	
	template <class T>
	TypedModel<T>* ModelBuilder<T>::build()
	{
		if (_auto_key)
		{
			return _auto_key->build(this);
		}
		
		TypedModel<T>* mod = new TypedModel<T>(Utils::move(props), _entity, Utils::move(bases), _key_name, _name);
		return mod;
	}
	
	
	template <class T>
	ModelBuilder<T>& ModelBuilder<T>::add(const string& name, auto field)
	{
		typedef detail::field_decomp<decltype(field)> dec;
		typedef typename dec::value F;
		assert(props.count(name) == 0);
		props[name] = new MemberProperty<type, F>(name, field);
		return *this;
	}
	
	
}
}
}

#endif