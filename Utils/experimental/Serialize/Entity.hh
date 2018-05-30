#ifndef INCLUDED_EXPERIMENTAL_SERIALIZE_ENTITY_HH
#define INCLUDED_EXPERIMENTAL_SERIALIZE_ENTITY_HH

#include <Common.h>
#include "Context.hh"
#include <Utils/utility>
#include <Utils/experimental/reflect>
#include "Adapter.hh"

namespace Utils { namespace experimental {
namespace serialize
{
	
	
	class Entity
	{
		protected:
		Context* _context;
		
		public:
		Entity(Context& c) : _context(&c)
		{
			
		}
		
		const Context* context() const noexcept
		{
			return _context;
		}
		
		virtual const reflect::Model* model() const noexcept = 0;
		
		virtual ValueSet* get_values() const = 0;
		virtual void set_values(const ValueSet*) = 0;
		
		virtual Plug<>* plug() = 0;
	};
	
	
	
	
	
	template <class T, class B = T>
	class _Entity : public T, public Entity
	{
		static_assert(Utils::is_polymorphic<T>::value);
		static_assert(!Utils::is_abstract<T>::value);
		static_assert(Utils::is_base_of<B, T>::value);
		public:
		typedef T type;
		typedef B base;
		
		protected:
		const reflect::TypedModel<base>* _model;
		Plug<base>* _plug;
		
		
		public:
		
		template <class... Args>
		_Entity(Context& c, const reflect::TypedModel<base>* m, Args... args) : type(forward<Args>(args)...), Entity(c), _model(m)
		{}
		
		virtual const reflect::TypedModel<base>* model() const noexcept override
		{
			return _model;
		}
		
		virtual ObjectValueSet<base>* get_values() const override
		{
			return ObjectValueSet<base>::Gather(model(), this);
		}
		
		virtual void set_values(const ValueSet* v) override
		{
			auto dyn = dynamic_cast<const ObjectValueSet<base>*>(v);
			if (dyn)
			{
				dyn->restore(model(), this);
				return;
			}
			assert(NOT_IMPLEMENTED);
		}
		
		virtual Plug<base>* plug() override
		{
			if (!_plug)
			{
				_plug = Plug<base>::From_Model(model(), this);
				assert(NOT_IMPLEMENTED);
			}
			return _plug;
		}
	};
	
	
	
	
}
}
}

#endif