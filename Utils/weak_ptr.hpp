#ifndef INCLUDED_WEAK_PTR_HPP
#define INCLUDED_WEAK_PTR_HPP

#include "weak_ptr.hh"
#include "detail/shared_ptr_control.hh"
#include "shared_ptr.hh"
#include "detail/shared_ptr_control.hh"

namespace Utils
{
	template <class T>
	constexpr weak_ptr<T>::weak_ptr() noexcept : ctrl(nullptr), ptr(nullptr)
	{
		
	}
	
	template <class T>
	constexpr weak_ptr<T>::weak_ptr(nullptr_t n) noexcept : ctrl(n), ptr(n)
	{
		
	}
	
	template <class T>
	template <class Y>
	weak_ptr<T>::weak_ptr(const shared_ptr<Y>& r) noexcept : ctrl(r.ctrl), ptr(r.get())
	{
		if (ctrl)
		{
			++ctrl->refcount;
		}
	}
	
	
	
	template <class T>
	weak_ptr<T>::~weak_ptr()
	{
		reset();
	}
	
	
	template <class T>
	void weak_ptr<T>::assign(detail::shared_ptr_control* ctrl, T* ptr)
	{
		if (ctrl == this->ctrl)
		{
			this->ptr = ptr;
			return;
		}
		reset();
		this->ctrl = ctrl;
		this->ptr = ptr;
		if (ctrl)
		{
			++ctrl->refcount;
		}
	}
	
	
	
	
	template <class T>
	void weak_ptr<T>::reset()
	{
		if (ctrl)
		{
			if (!ctrl->weakRelease())
			{
				detail::shared_ptr_control::Release(ctrl);
			}
		}
		ctrl = nullptr;
	}
	
	template <class T>
	long weak_ptr<T>::use_count() const noexcept
	{
		if (ctrl != nullptr)
		{
			return ctrl->usecount;
		}
		return 0;
	}
	
	template <class T>
	bool weak_ptr<T>::expired() const noexcept
	{
		if (ctrl)
		{
			return ctrl->usecount == 0;
		}
		return true;
	}
	
	template <class T>
	shared_ptr<T> weak_ptr<T>::lock() const noexcept
	{
		return expired() ? shared_ptr<T>(nullptr) : shared_ptr<T>(ctrl, ptr);
	}
	
	
	
	
	
	
	
	template <class T>
	weak_ptr<T>& weak_ptr<T>::operator=(const weak_ptr& r)
	{
		auto ctrl2 = r.ctrl;
		auto ptr2 = r.ptr;
		if (ctrl2)
		{
			++ctrl2->refcount;
		}
		reset();
		ctrl = ctrl2;
		ptr = ptr2;
		return *this;
	}
	
}

#endif