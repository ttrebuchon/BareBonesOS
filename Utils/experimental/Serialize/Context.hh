#ifndef INCLUDED_EXPERIMENTAL_SERIALIZE_CONTEXT_HH
#define INCLUDED_EXPERIMENTAL_SERIALIZE_CONTEXT_HH

#include <Common.h>
#include "Adapter.hh"

namespace Utils::experimental::reflect
{
	class TypeSystem;
}

namespace Utils { namespace experimental {
namespace serialize
{
	
	namespace detail
	{
		struct type_pair
		{
			const type_info* t1;
			const type_info* t2;
		};
		
		bool operator==(const type_pair& lhs, const type_pair& rhs) noexcept
		{
			return (lhs.t1 == rhs.t1 && lhs.t2 == rhs.t2) || (lhs.t1 == rhs.t2 && lhs.t2 == rhs.t1);
		}
		
		struct type_pair_hasher
		{
			size_t operator()(const type_pair& p) const
			{
				return p.t1->hash_code() + p.t2->hash_code();
			}
		};
	}
	
	
	class Context
	{
		protected:
		reflect::TypeSystem* _ts;
		unordered_map<type_index, Adapter<>*> adapters;
		
		virtual bool save_values(const reflect::Model*, list<ValueSet*>&) = 0;
		virtual bool save(const reflect::Model*, void* obj) {
			assert(NOT_IMPLEMENTED);
			}
		
		bool _save(map<const reflect::Model*, list<ValueSet*>>&& _values)
		{
			map<const reflect::Model*, list<ValueSet*>> values(_values);
			bool status = true;
			
			for (auto& kv : values)
			{
				assert(kv.first->is_entity());
				status &= save_values(kv.first, kv.second);
				for (auto v : kv.second)
				{
					delete v;
				}
				if (!status)
				{
					break;
				}
			}
			
			return status;
		}
		
		// DEBUG
		public:
		
		
		template <class T>
		const Adapter<T>* adapter() const
		{
			//detail::type_pair tp{&t1, &t2};
			type_index k(typeid(T));
			#if __EXCEPTIONS
			return dynamic_cast<const Adapter<T>*>(adapters.at(k));
			#else
			if (adapters.count(k))
			{
				return dynamic_cast<const Adapter<T>*>(adapters.at(k));
			}
			return nullptr;
			#endif
		}
		
		public:
		Context() : _ts(new reflect::TypeSystem)
		{
			
		}
		
		virtual const reflect::TypeSystem& types() const noexcept
		{
			return *_ts;
		}
		
		template <class T>
		void addModel(reflect::ModelBuilder_fn<T> mb)
		{
			assert(_ts);
			_ts->addModel(mb);
			assert(types().get_model<T>());
			auto ad = ModelAdapter<T>::Create(types());
			assert(ad);
			
			adapters[typeid(T)] = ad;
			
		}
		
		bool link()
		{
			return _ts->link();
		}
		
		
		template <class, class>
		friend class Collection;
	};
	
}
}
}

#endif