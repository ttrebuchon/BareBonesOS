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
	constexpr shared_ptr<T>::shared_ptr() noexcept : ctrl(nullptr), other(nullptr)
	{
		
	}
	
	template <class T>
	constexpr shared_ptr<T>::shared_ptr(nullptr_t n) noexcept : ctrl(n), other(n)
	{
		
	}
	
	template <class T>
	template <class Y, class Deleter>
	shared_ptr<T>::shared_ptr(Y* ptr, Deleter d) : ctrl(detail::use_SFT<T, Y>::use(ptr)), other(nullptr)
	{
		if (!ctrl)
		{
			ctrl = detail::shared_ptr_control::CreateControl(ptr, d);
			detail::use_SFT<T, Y>::set(ptr, ctrl);
		}
	}
	
	template <class T>
	template <class Y, class Deleter, class Alloc>
	shared_ptr<T>::shared_ptr(Y* ptr, Deleter d, const Alloc& alloc) : ctrl(detail::use_SFT<T, Y>::use(ptr)), other(nullptr)
	{
		if (!ctrl)
		{
			ctrl = detail::shared_ptr_control::CreateControl(ptr, forward<Deleter>(d), forward<const Alloc&>(alloc));
			detail::use_SFT<T, Y>::set(ptr, ctrl);
		}
	}
	
	template <class T>
	template <class Y>
	shared_ptr<T>::shared_ptr(const shared_ptr<Y>& other, element_type* value) : shared_ptr(other.ctrl, value)
	{
		
	}
	
	template <class T>
	template <class Y, class>
	shared_ptr<T>::shared_ptr(Y* ptr) : ctrl(detail::use_SFT<T, Y>::use(ptr)), other(nullptr)
	{
		if (!ctrl)
		{
			ctrl = detail::shared_ptr_control::CreateControl(ptr);
			detail::use_SFT<T, Y>::set(ptr, ctrl);
		}
	}
	
	template <class T>
	template <class Y, class>
	shared_ptr<T>::shared_ptr(const shared_ptr<Y>& r) noexcept : ctrl(r.ctrl), other(r.other)
	{
		//T* x = get();
		//x = r.get();
		
		
		++ctrl->refcount;
		++ctrl->usecount;
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(const shared_ptr& r) noexcept : ctrl(r.ctrl), other(r.other)
	{
		if (ctrl)
		{
			++ctrl->refcount;
			++ctrl->usecount;
		}
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(shared_ptr&& r) noexcept : ctrl(r.ctrl), other(r.other)
	{
		r.ctrl = nullptr;
		r.other = nullptr;
	}
	
	template <class T>
	template <class Y, class>
	shared_ptr<T>::shared_ptr(shared_ptr<Y>&& r) noexcept : ctrl(r.ctrl), other(r.other)
	{
		r.ctrl = nullptr;
		r.other = nullptr;
		/*++ctrl->refcount;
		++ctrl->usecount;*/
	}
	
	template <class T>
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
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(detail::shared_ptr_control* ctrl) : ctrl(ctrl), other(nullptr)
	{
		if (ctrl)
		{
			++ctrl->refcount;
			++ctrl->usecount;
		}
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(detail::shared_ptr_control* ctrl, element_type* value) : ctrl(ctrl), other(value)
	{
		assert(ctrl || !value);
		if (ctrl)
		{
			++ctrl->refcount;
			++ctrl->usecount;
		}
	}
	
	template <class T>
	template <class Y>
	shared_ptr<T>::shared_ptr(const weak_ptr<Y>& weak) : ctrl(weak.ctrl), other(weak.ptr)
	{
		if (!ctrl || !other)
		{
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
	typename shared_ptr<T>::ptr_type shared_ptr<T>::get() const noexcept
	{
		if (unlikely(other != nullptr))
		{
			return other;
		}
		return (ctrl != nullptr ? (T*)ctrl->obj : nullptr);
	}
	
	template <class T>
	void shared_ptr<T>::reset()
	{
		if (ctrl)
		{
			if (!ctrl->strongRelease())
			{
				if (ctrl->dealloc)
				{
					ctrl->dealloc(ctrl->dealloc_object, ctrl);
				}
				else
				{
					delete ctrl;
				}
			}
		}
		ctrl = nullptr;
		other = nullptr;
	}
	
	template <class T>
	long shared_ptr<T>::use_count() const noexcept
	{
		if (ctrl != nullptr)
		{
			if (ctrl->obj != nullptr)
			{
				return ctrl->usecount;
			}
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
		other = r.other;
		return *this;
	}
	
	template <class T>
	shared_ptr<T>::operator bool() const noexcept
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