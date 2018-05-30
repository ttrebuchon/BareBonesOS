#ifndef INCLUDED_EXPERIMENTAL_SERIALIZE_PRONGFACTORY_HH
#define INCLUDED_EXPERIMENTAL_SERIALIZE_PRONGFACTORY_HH

#include <Common.h>



namespace Utils { namespace experimental {
namespace serialize
{
	template <class T = void, class Y = void, class V = void>
	struct ProngFactory;
	
	template <class T>
	struct ProngFactory<T, void, void>
	{
		static Prong<>* Call(const reflect::Property<T>* p, T* obj, const reflect::TypedModel<T>* model)
		{
			if (p->is_field())
			{
				
				//auto& v = 
			}
			assert(NOT_IMPLEMENTED);
		}
	};
	
	template <class T, class V>
	struct ProngFactory<T, void, V>
	{
		static Prong<>* Call(const reflect::Property<T>*, T* obj, const reflect::TypedModel<V>* model);
	};
	
	template <class T, class Y>
	struct ProngFactory<T, Y, void>
	{
		static Prong<>* Call(const reflect::TypedProperty<T, Y>* p, T* obj, const reflect::TypedModel<T>* model)
		{
			if (p->is_field())
			{
				auto fp = dynamic_cast<const reflect::FieldBasedProperty<T, Y>*>(p);
				assert(fp);
				auto& v = fp->value(*obj);
				
				return new PointerProng<Y>(&v);
			}
			assert(NOT_IMPLEMENTED);
		}
	};
	
	template <class T, class Y, class V>
	struct ProngFactory
	{
		static Prong<>* Call(const reflect::TypedProperty<T, Y>*, T* obj, const reflect::TypedModel<V>* model);
	};
}
}
}


#endif