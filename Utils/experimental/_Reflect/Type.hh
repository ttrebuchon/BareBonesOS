#ifndef INCLUDED_EXPERIMENTAL_REFLECT_TYPE_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_TYPE_HH

#include <Common.h>
#include <Utils/type_traits>
#include <Utils/map>
#include <Utils/typeindex>
#include <Utils/memory>
#include "Primitive.hh"

namespace Utils { namespace experimental {
namespace reflect
{
	template <class>
	class TypedType;
	
	
	class Type
	{
		public:
		typedef const std::type_info& info_type;
		
		
		protected:
		static map<type_index, const Type*> cache;
		
		Type() = default;
		
		public:
		
		
		
		virtual info_type info() const noexcept = 0;
		virtual bool is_pointer() const noexcept = 0;
		virtual bool is_const() const noexcept = 0;
		virtual bool is_volatile() const noexcept = 0;
		virtual bool is_reference() const noexcept = 0;
		virtual bool is_class() const noexcept = 0;
		virtual bool is_abstract() const noexcept = 0;
		
		
		virtual FundamentalType fundamental() const noexcept = 0;
		virtual const Type* under() const noexcept = 0;
		const Type* bottom() const noexcept
		{
			auto ptr = this->under();
			if (!ptr)
			{
				return this;
			}
			auto tmp = ptr;
			while (tmp)
			{
				ptr = tmp;
				tmp = ptr->under();
			}
			return ptr;
		}
		
		template <class T>
		static const TypedType<T>* Get();
		static const Type* Get(info_type);
	};
	
	
	template <class T>
	class TypedType : public Type
	{
		public:
		typedef typename Type::info_type info_type;
		typedef T type;
		
		protected:
		
		public:
		
		
		
		virtual info_type info() const noexcept override
		{
			return typeid(type);
		}
		
		virtual bool is_pointer() const noexcept override
		{
			return Utils::is_pointer<type>::value;
		}
		
		virtual bool is_const() const noexcept override
		{
			return Utils::is_const<type>::value;
		}
		
		virtual bool is_volatile() const noexcept override
		{
			return Utils::is_volatile<type>::value;
		}
		
		virtual bool is_reference() const noexcept override
		{
			return Utils::is_reference<type>::value;
		}
		
		virtual bool is_class() const noexcept override
		{
			return Utils::is_class<type>::value;
		}
		
		virtual bool is_abstract() const noexcept override
		{
			return Utils::is_abstract<type>::value;
		}
		
		virtual FundamentalType fundamental() const noexcept override
		{
			return Fundamental<type>::value;
		}
		
		virtual const Type* under() const noexcept = 0;
		
	};
	
	template <class T>
	class _SpecTypedType : public TypedType<T>
	{
		public:
		typedef typename Type::info_type info_type;
		typedef T type;
		
		private:
		static const _SpecTypedType<type> _singleton;
		
		public:
		
		static const TypedType<type>* Get()
		{
			return &_singleton;
		}
		
		virtual const Type* under() const noexcept
		{
			return nullptr;
		}
		
	};
	
	template <class T>
	const _SpecTypedType<T> _SpecTypedType<T>::_singleton;
	
	template <class T>
	class _SpecTypedType<T*> : public TypedType<T*>
	{
		
		public:
		typedef typename Type::info_type info_type;
		typedef T* type;
		typedef typename Utils::remove_pointer<type>::type beneath_type;
		
		private:
		static const _SpecTypedType<type> _singleton;
		
		protected:
		mutable bool _checked = false;
		mutable const TypedType<beneath_type>* _below = nullptr;
		
		public:
		
		static const TypedType<type>* Get()
		{
			return &_singleton;
		}
		
		virtual const Type* under() const noexcept override
		{
			if (!_checked)
			{
				_below = _SpecTypedType<beneath_type>::Get();
				_checked = true;
			}
			return _below;
		}
		
	};
	template <class T>
	const _SpecTypedType<T*> _SpecTypedType<T*>::_singleton;
	
	
	
	
	
	
	template <class T>
	class _SpecTypedType<const T> : public TypedType<const T>
	{
		
		public:
		typedef typename Type::info_type info_type;
		typedef const T type;
		typedef T beneath_type;
		
		private:
		static const _SpecTypedType<type> _singleton;
		
		protected:
		mutable bool _checked = false;
		mutable const TypedType<beneath_type>* _below = nullptr;
		
		public:
		
		static const TypedType<type>* Get()
		{
			return &_singleton;
		}
		
		virtual const Type* under() const noexcept override
		{
			if (!_checked)
			{
				_below = _SpecTypedType<beneath_type>::Get();
				_checked = true;
			}
			return _below;
		}
		
	};
	template <class T>
	const _SpecTypedType<const T> _SpecTypedType<const T>::_singleton;
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	template <class T>
	const TypedType<T>* Type::Get()
	{
		type_index i(typeid(T));
		if (!cache.count(i))
		{
			cache[i] = _SpecTypedType<T>::Get();
		}
		//return cache.at(i);
		return _SpecTypedType<T>::Get();
	}
}
}
}

#endif