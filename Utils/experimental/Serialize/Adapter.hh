#ifndef INCLUDED_EXPERIMENTAL_SERIALIZE_ADAPTER_HH
#define INCLUDED_EXPERIMENTAL_SERIALIZE_ADAPTER_HH

#include <Common.h>



namespace Utils { namespace experimental {
namespace serialize
{
	namespace detail
	{
		template <class T>
		struct delete_if_pointer
		{
			static inline bool call(const T& t, bool setNull = true)
			{
				return false;
			}
		};
		
		template <class T>
		struct delete_if_pointer<T*>
		{
			static inline bool call(T*& t, bool setNull = true)
			{
				if (!t)
				{
					return false;
				}
				delete t;
				if (setNull)
				{
					t = nullptr;
				}
				return true;
			}
		};
	};
	
	
	template <class Y = void>
	class Prong;
	
	template <>
	class Prong<void>
	{
		protected:
		
		
		public:
		
		virtual const reflect::Type* get_type() const = 0;
		bool is_pointer() const
		{
			auto t = get_type();
			if (t)
			{
				return t->is_pointer();
			}
			else
			{
				assert(NOT_IMPLEMENTED);
				return false;
			}
		}
		virtual bool delete_pointer() = 0;
		
	};
	
	template <class Y>
	class Prong : virtual public Prong<>
	{
		public:
		typedef Y type;
		
		protected:
		
		public:
		
		const reflect::Type* get_type() const override
		{
			return reflect::Type::Get<Y>();
		}
	};
	
	template <class T>
	class ObjectProng : virtual public Prong<>
	{
		public:
		typedef typename Utils::decay<T>::type source_type;
		
		protected:
		source_type* src;
		
		
		
		public:
		
		void set_source(source_type* src) noexcept
		{
			this->src = src;
		}
		
		source_type* get_source() noexcept
		{
			return src;
		}
	};
	
	template <class Y>
	class PointerProng : public Prong<Y>
	{
		public:
		using typename Prong<Y>::type;
		
		protected:
		Y* _pointer;
		
		
		
		public:
		PointerProng(Y* ptr) : _pointer(ptr)
		{
			
		}
		
		
		virtual bool delete_pointer() override
		{
			if (_pointer)
			{
				return detail::delete_if_pointer<Y>::call(*_pointer);
			}
			
			return false;
		}
	};
	
	template <class Y, class T>
	class PropertyProng : public Prong<Y>, public ObjectProng<T>
	{
		public:
		using typename Prong<Y>::type;
		using typename ObjectProng<T>::source_type;
		
		protected:
		const reflect::TypedProperty<source_type, type>* _property;
		
		
		
		public:
		
		
	};
	
	template <class T = void>
	class Plug;
	
	template <>
	class Plug<void>
	{
		public:
		typedef string key_type;
		
		protected:
		
		public:
		
		virtual Prong<>* prong(const key_type&) const = 0;
	};
	
	template <class T>
	class Plug : public Plug<void>
	{
		public:
		typedef typename Utils::decay<T>::type source_type;
		using typename Plug<>::key_type;
		
		protected:
		map<key_type, Prong<>*> prongs;
		
		public:
		
		static Plug* From_Model(const reflect::TypedModel<source_type>* model, source_type* obj)
		{
			assert(model);
			
			Plug<T>* plg = new Plug<T>;
			
			for (const auto& kv : model->properties())
			{
				const auto& p = kv.second;
				auto pr = p->make_prong(model, obj);
				assert(pr);
				assert(!plg->prongs.count(kv.first));
				plg->prongs[kv.first] = pr;
			}
			return plg;
		}
		
		virtual Prong<>* prong(const key_type& k) const override
		{
			return prongs.at(k);
		}
	};
	
	
	
	template <class T = void>
	class Adapter;
	
	template <>
	class Adapter<void>
	{
		protected:
		
		public:
		
		virtual ~Adapter() = default;
		
	};
	
	template <class T>
	class Adapter : public Adapter<>
	{
		public:
		typedef typename Utils::decay<T>::type type;
		
		protected:
		//Plug<type>* _p1;
		//Plug<>* _p2;
		
		public:
		
		virtual bool connect(Plug<type>* lhs, Plug<>* rhs) const = 0;
		virtual bool connect(Plug<>* lhs, Plug<type>* rhs) const = 0;
		
	};
	
	/*template <class T1, class T2>
	class ObjectAdapter : public Adapter<T1, T2>
	{
		public:
		using typename Adapter<T1, T2>::type1;
		using typename Adapter<T1, T2>::type2;
		
		protected:
		const reflect::TypedModel<type1>* _model;
		const reflect::TypedModel<type2>* _model2;
		
		
		public:
		virtual bool connect(Plug<type1>* lhs, Plug<type2>* rhs) const override;
	};*/
	
	
	template <class T>
	class ModelAdapter : public Adapter<T>
	{
		public:
		using typename Adapter<T>::type;
		
		protected:
		const reflect::TypedModel<type>* _model;
		
		class ModelProng : public Prong<>
		{
			public:
			Prong<>* src;
			
			
		};
		
		class PrimitiveProng : public ModelProng
		{
			public:
			
			
		};
		
		
		public:
		ModelAdapter(const reflect::TypedModel<type>* _model) : _model(_model)
		{
			assert(_model);
			
			if (_model->is_entity())
			{
				
				auto key = _model->get_key();
				
			}
		}
		
		ModelAdapter(const reflect::TypeSystem& ts) : ModelAdapter(ts.get_model<type>())
		{}
		
		
		virtual bool connect(Plug<type>* lhs, Plug<>* rhs) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual bool connect(Plug<>* lhs, Plug<type>* rhs) const override
		{
			assert(lhs);
			assert(rhs);
			
			const reflect::Model* keyModel;
			auto key = _model->get_key(&keyModel);
			assert(key);
			assert(keyModel);
			
			if (keyModel != _model)
			{
				auto keyLeft = lhs->prong(keyModel->name());
				auto keyRight = rhs->prong(keyModel->name());
				
				assert(keyLeft);
				assert(keyRight);
				
				// keyLeft->pull(keyRight);
				
			}
			
			for (const auto& kv : _model->properties())
			{
				auto l = lhs->prong(kv.first);
				auto r = rhs->prong(kv.first);
				assert(l);
				assert(r);
				
				if (!l || !r)
				{
					return false;
				}
				
				
				// l->pull(r);
				
			}
			assert(NOT_IMPLEMENTED);
		}
		
		
		static ModelAdapter* Create(const reflect::TypeSystem& ts)
		{
			auto model = ts.get_model<type>();
			assert(model);
			
			return Create(model);
		}
		
		static ModelAdapter* Create(const reflect::TypedModel<type>* _model);
		
	};
	
	
	template <class T>
	class EntityModelAdapter : public ModelAdapter<T>
	{
		public:
		using typename ModelAdapter<T>::type;
		
		protected:
		
		public:
		EntityModelAdapter(const reflect::TypedModel<type>* _model) : ModelAdapter<T>(_model)
		{
			assert(this->_model);
			assert(_model->is_entity());
			
			auto key = _model->get_key();
			
		}
		
		EntityModelAdapter(const reflect::TypeSystem& ts) : EntityModelAdapter(ts.get_model<type>())
		{}
	};
	
	template <class T>
	ModelAdapter<T>* ModelAdapter<T>::Create(const reflect::TypedModel<type>* model)
	{
		
		assert(model);
		if (model->is_entity())
		{
			
			return new EntityModelAdapter<T>(model);
		}
		else
		{
			
			return new ModelAdapter<T>(model);
		}
	}
}
}
}

#endif