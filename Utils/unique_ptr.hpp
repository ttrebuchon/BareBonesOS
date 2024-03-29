#ifndef INCLUDED_UNIQUE_PTR_HPP
#define INCLUDED_UNIQUE_PTR_HPP

#include "unique_ptr.hh"

namespace Utils
{
	
	template <class T, class Deleter>
	constexpr unique_ptr<T, Deleter>::unique_ptr() noexcept : obj(nullptr)
	{
		
	}
	
	template <class T, class Deleter>
	constexpr unique_ptr<T, Deleter>::unique_ptr(nullptr_t n) noexcept : obj(n)
	{
		
	}
	
	template <class T, class Deleter>
	unique_ptr<T, Deleter>::unique_ptr(pointer p) : obj(p)
	{
		
	}
	
	template <class T, class Deleter>
	unique_ptr<T, Deleter>::unique_ptr(pointer ptr, typename detail::unique_ptr_deleter_args<Deleter>::type1 d1) noexcept : obj(ptr), del(d1)
	{
		
	}
	
	template <class T, class Deleter>
	unique_ptr<T, Deleter>::unique_ptr(pointer ptr, typename detail::unique_ptr_deleter_args<Deleter>::type2 d2) noexcept : obj(ptr), del(d2)
	{
		
	}
	
	template <class T, class Deleter>
	template <class U, class E>
	unique_ptr<T, Deleter>::unique_ptr(unique_ptr<U, E>&& r) noexcept : obj(Utils::move(r.obj)), del(Utils::move(r.del))
	{
		r.obj = nullptr;
	}
	
	template <class T, class Deleter>
	unique_ptr<T, Deleter>::unique_ptr(unique_ptr&& r) noexcept : obj(r.obj), del(r.del)
	{
		r.obj = nullptr;
	}
	
	
	template <class T, class Deleter>
	unique_ptr<T, Deleter>::~unique_ptr() noexcept
	{
		reset();
	}
	
	
	
	
	template <class T, class Deleter>
	auto unique_ptr<T, Deleter>::release() noexcept -> pointer
	{
		auto other = obj;
		obj = nullptr;
		return other;
	}
	
	template <class T, class Deleter>
	void unique_ptr<T, Deleter>::reset() noexcept
	{
		if (obj != nullptr)
		{
			del(obj);
			obj = nullptr;
			del = Utils::move(Deleter());
		}
	}
	
	template <class T, class Deleter>
	void unique_ptr<T, Deleter>::swap(unique_ptr& other) noexcept
	{
		auto _obj = this->obj;
		auto _del = this->del;
		this->obj = other.obj;
		this->del = other.del;
		other.obj = _obj;
		other.del = _del;
	}
	
	template <class T, class Deleter>
	auto unique_ptr<T, Deleter>::get() const noexcept -> pointer
	{
		return obj;
	}
	
	
	
	
	
	
	
	template <class T, class Deleter>
	auto unique_ptr<T, Deleter>::operator=(unique_ptr&& p) noexcept -> unique_ptr&
	{
		reset();
		obj = p.obj;
		p.obj = nullptr;
		del = Utils::move(p.del);
		
		return *this;
	}

	template <class T, class Deleter>
	auto unique_ptr<T, Deleter>::operator=(nullptr_t) noexcept -> unique_ptr&
	{
		reset();
		return *this;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	template <class T>
	struct make_unique_t
	{
		template <class... Args>
		static unique_ptr<T> make(Args&&... args)
		{
			typedef void(*DelPtr)(T*);
			
			
			void* raw = ::operator new (sizeof(T));
			unique_ptr<T> ptr((T*)raw);
			new (raw) T(args...);
			return ptr;
		}
	};
	
	template <class T>
	struct make_unique_t<T[]>
	{
		static unique_ptr<T> make(size_t n)
		{
			/*void* raw = ::operator new ((n*sizeof(T) + sizeof(detail::shared_ptr_control))/sizeof(char));
			shared_ptr<T> ptr(raw, n*sizeof(T) + sizeof(detail::shared_ptr_control));
			new (raw + sizeof(detail::shared_ptr_control)) T(args...);
			return ptr;*/
		}
	};
	
	
	
	template <class T, class... Args>
	inline unique_ptr<T> make_unique(Args&&... args)
	{
		return make_unique_t<T>::make(forward<Args>(args)...);
	}
	
}

#endif