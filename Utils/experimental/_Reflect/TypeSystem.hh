#ifndef INCLUDED_EXPERIMENTAL_REFLECT_TYPESYSTEM_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_TYPESYSTEM_HH

#include <Common.h>
#include "Properties.hh"
#include <Utils/unordered_map>
#include <Utils/typeindex>
#include "ModelBuilder.hh"
#include "TypeModel.hh"

namespace Utils { namespace experimental {
namespace reflect
{
	
	class TypeSystem
	{
		protected:
		unordered_map<type_index, Model*> models;
		
		bool _linked = false;
		
		public:
		
		
		
		
		bool link();
		
		template <class T>
		void addModel(ModelBuilder_fn<T> fn)
		{
			if (!models.count(typeid(T)))
			{
				auto model = new ModelBuilder<T>();
				fn(*model);
				
				models[typeid(T)] = model->build();
				assert(models[typeid(T)]);
				delete model;
			}
			else
			{
				#ifdef __EXCEPTIONS
				// TODO
				throw exception();
				#else
				// TODO
				assert(NOT_IMPLEMENTED);
				#endif
			}
		}
		
		bool is_satisfied() const
		{
			for (auto& modelPair : models)
			{
				if (!modelPair.second->is_satisfied(*this))
				{
					return false;
				}
			}
			return true;
		}
		
		bool has_model_for(const type_index& ind) const noexcept
		{
			return models.count(ind) > 0;
		}
		
		template <class T>
		const TypedModel<typename Utils::decay<T>::type>* get_model() const
		{
			typedef typename Utils::decay<T>::type type;
			type_index i(typeid(type));
			if (models.count(i))
			{
				return (const TypedModel<type>*)models.at(i);
			}
			return nullptr;
		}
		
		const Model* get_model(const type_info& tinfo) const
		{
			type_index i(tinfo);
			if (models.count(i))
			{
				return models.at(i);
			}
			return nullptr;
		}
	};
	
}
}
}

#endif