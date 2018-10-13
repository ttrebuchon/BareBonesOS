#ifndef INCLUDED_SHARED_PTR_HPP
#define INCLUDED_SHARED_PTR_HPP

#include "shared_ptr.hh"
#include "detail/shared_ptr_control.hh"
#include "detail/shared_ptr_control.hpp"
#include "detail/forward.hh"
#include <Utils/utility>
#include "detail/bits/enable_shared_from_this.hh"
#include "detail/bits/detect_shared_from_this.hh"

namespace Utils
{
	
	template <class T>
	constexpr shared_ptr<T>::shared_ptr() noexcept : ctrl(nullptr), obj(nullptr)
	{
		
	}
	
	template <class T>
	constexpr shared_ptr<T>::shared_ptr(nullptr_t n) noexcept : ctrl(n), obj(n)
	{
		
	}
	
	template <class T>
	template <class Y, class Deleter>
	shared_ptr<T>::shared_ptr(Y* ptr, Deleter d) : ctrl(nullptr), obj(nullptr)
	{
		if (!ptr)
		{
			return;
		}
		
		if (!shared_from_this_helper_get(ptr, ptr))
		{
			ctrl = detail::shared_ptr_control::CreateControl(ptr, forward<Deleter>(d));
			obj = ptr;
			shared_from_this_helper_set(ctrl, ptr, ptr);
			
		}
	}
	
	template <class T>
	template <class Y, class Deleter, class Alloc>
	shared_ptr<T>::shared_ptr(Y* ptr, Deleter d, const Alloc& alloc) : ctrl(nullptr), obj(nullptr)
	{
		if (!ptr)
		{
			return;
		}
		
		if (!shared_from_this_helper_get(ptr, ptr))
		{
			ctrl = detail::shared_ptr_control::CreateControl(ptr, forward<Deleter>(d), forward<const Alloc&>(alloc));
			obj = ptr;
			shared_from_this_helper_set(ctrl, ptr, ptr);
			
		}
	}
	
	template <class T>
	template <class Y>
	shared_ptr<T>::shared_ptr(const shared_ptr<Y>& other, element_type* value) : shared_ptr(other.ctrl, value)
	{
		
	}
	
	template <class T>
	template <class Y, class>
	shared_ptr<T>::shared_ptr(Y* ptr) : ctrl(nullptr), obj(nullptr)
	{
		if (!shared_from_this_helper_get(ptr, ptr))
		{
			ctrl = detail::shared_ptr_control::CreateControl((T*)ptr);
			obj = ptr;
			shared_from_this_helper_set(ctrl, ptr, ptr);
			
		}
	}
	
	template <class T>
	template <class Y, class>
	shared_ptr<T>::shared_ptr(const shared_ptr<Y>& r) noexcept : ctrl(r.ctrl), obj(r.get())
	{
		if (ctrl)
		{
			++ctrl->refcount;
			++ctrl->usecount;
		}
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(const shared_ptr& r) noexcept : ctrl(r.ctrl), obj(r.obj)
	{
		if (ctrl)
		{
			++ctrl->refcount;
			++ctrl->usecount;
		}
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(shared_ptr&& r) noexcept : ctrl(r.ctrl), obj(r.obj)
	{
		r.ctrl = nullptr;
		r.obj = nullptr;
	}
	
	template <class T>
	template <class Y, class>
	shared_ptr<T>::shared_ptr(shared_ptr<Y>&& r) noexcept : ctrl(r.ctrl), obj(r.obj)
	{
		r.ctrl = nullptr;
		r.obj = nullptr;
	}
	
	/*template <class T>
	template <class Deleter>
	shared_ptr<T>::shared_ptr(void* raw, size_t nbytes, Deleter d) : ctrl(nullptr), other(nullptr)
	{
		ctrl = ::new (raw) detail::shared_ptr_control;
		ctrl->refcount = ctrl->usecount = 1;
		
		void* rawDest = (char*)raw + sizeof(detail::shared_ptr_control)/sizeof(char);
		void* rawObj = (char*)rawDest + sizeof(detail::shared_ptr_control::Destructor<T, Deleter>)/sizeof(char);
		
		
		auto dest = new (rawDest) detail::shared_ptr_control::template Destructor<T, Deleter>(d);
		ctrl->deleter_obj = dest;
		ctrl->deleter = detail::shared_ptr_control::template Destructor<T, Deleter>::call;
		
		ctrl->obj = rawObj;
	}*/
	
	template <class T>
	shared_ptr<T>::shared_ptr(detail::shared_ptr_control* ctrl) : ctrl(ctrl), obj(nullptr)
	{
		assert(NOT_IMPLEMENTED);
		if (ctrl)
		{
			++ctrl->refcount;
			++ctrl->usecount;
		}
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(detail::shared_ptr_control* ctrl, element_type* value) : ctrl(ctrl), obj(value)
	{
		if (!ctrl)
		{
			assert(!value);
			value = nullptr;
		}
		assert(ctrl || !value);
		if (ctrl)
		{
			++ctrl->refcount;
			++ctrl->usecount;
		}
	}
	
	
	template <class T>
	template <class SFT, class Y>
	bool shared_ptr<T>::shared_from_this_helper_set(detail::shared_ptr_control* _ctrl, enable_shared_from_this<SFT>* sft, Y* ptr)
	{
		static_assert(detail::use_SFT<SFT, Y>::has_SFT);
		detail::use_SFT<SFT, Y>::set(ptr, _ctrl);
		return true;
	}
	
	template <class T>
	template <class SFT, class Y>
	bool shared_ptr<T>::shared_from_this_helper_get(enable_shared_from_this<SFT>* sft, Y* ptr)
	{
		static_assert(detail::use_SFT<SFT, Y>::has_SFT);
		ctrl = detail::use_SFT<SFT, Y>::use(ptr);
		if (ctrl != nullptr)
		{
			++ctrl->refcount;
			++ctrl->usecount;
			obj = ptr;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	
	
	
	template <class T>
	template <class Y>
	shared_ptr<T>::shared_ptr(const weak_ptr<Y>& weak) : ctrl(weak.ctrl), obj(weak.ptr)
	{
		if (!ctrl || !obj)
		{
			TRACE_VAL(ctrl);
			TRACE_VAL((void*)obj);
			// TODO: Throw std::bad_weak_ptr
			assert(NOT_IMPLEMENTED);
		}
		
		++ctrl->refcount;
		++ctrl->usecount;
	}
	
	
	template <class T>
	shared_ptr<T>::~shared_ptr()
	{
		reset();
	}
	
	
	
	
	
	
	
	
	template <class T>
	constexpr typename shared_ptr<T>::ptr_type shared_ptr<T>::get() const noexcept
	{
		return obj;
	}
	
	template <class T>
	void shared_ptr<T>::reset()
	{
		if (ctrl)
		{
			if (!ctrl->strongRelease())
			{
				detail::shared_ptr_control::Release(ctrl);
			}
		}
		ctrl = nullptr;
		obj = nullptr;
	}
	
	template <class T>
	long shared_ptr<T>::use_count() const noexcept
	{
		if (ctrl != nullptr)
		{
			return ctrl->usecount;
		}
		return 0;
	}
	
	
	
	template <class T>
	shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr& r) noexcept
	{
		auto ctrl2 = r.ctrl;
		if (ctrl2)
		{
			++ctrl2->refcount;
			++ctrl2->usecount;
		}
		reset();
		ctrl = ctrl2;
		obj = r.obj;
		return *this;
	}
	
	template <class T>
	constexpr shared_ptr<T>::operator bool() const noexcept
	{
		return get() != nullptr;
	}
	
	template <class T>
	typename shared_ptr<T>::ref_type shared_ptr<T>::operator*() const noexcept
	{
		return *get();
	}
	
	template <class T>
	typename shared_ptr<T>::ptr_type shared_ptr<T>::operator->() const noexcept
	{
		return get();
	}
	
	template <class T>
	typename shared_ptr<T>::ref_type shared_ptr<T>::operator[](ptrdiff_t in) const
	{
		return get()[in];
	}
	
	
	
	
	
	
	
	
	template <class T>
	struct make_shared_t
	{
		template <class... Args>
		static shared_ptr<T> make(Args&&... args)
		{
			typedef void(*DelPtr)(T*);
			
			return shared_ptr<T>(new T(forward<Args&&>(args)...));
			
			/*void* raw = ::operator new ((sizeof(T) + sizeof(detail::shared_ptr_control::Destructor<T, DelPtr>) + sizeof(detail::shared_ptr_control))/sizeof(char));
			shared_ptr<T> ptr(raw,
				sizeof(T) + sizeof(detail::shared_ptr_control),
				(DelPtr)[](T* tptr)
				{
					if (tptr)
					{
						tptr->~T();
						delete (void*)tptr;
					}
					//delete tptr;
				}
				);
			new ((char*)raw +  (sizeof(detail::shared_ptr_control::Destructor<T, DelPtr>) + sizeof(detail::shared_ptr_control))/sizeof(char)) T(args...);
			return ptr;*/
		}
	};
	
	template <class T>
	struct make_shared_t<T[]>
	{
		static shared_ptr<T> make(size_t n)
		{
			/*void* raw = ::operator new ((n*sizeof(T) + sizeof(detail::shared_ptr_control))/sizeof(char));
			shared_ptr<T> ptr(raw, n*sizeof(T) + sizeof(detail::shared_ptr_control));
			new (raw + sizeof(detail::shared_ptr_control)) T(args...);
			return ptr;*/
			
			assert(NOT_IMPLEMENTED);
		}
	};
	
	
	
	template <class T, class... Args>
	inline shared_ptr<T> make_shared(Args&&... args)
	{
		return make_shared_t<T>::make(forward<Args&&>(args)...);
	}
	
}

#endif