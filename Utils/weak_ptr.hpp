#ifndef INCLUDED_WEAK_PTR_HPP
#define INCLUDED_WEAK_PTR_HPP

#include "weak_ptr.hh"
#include "detail/shared_ptr_control.hh"
#include "shared_ptr.hh"
#include "detail/shared_ptr_control.hh"

namespace Utils
{
	template <class T>
	constexpr weak_ptr<T>::weak_ptr() : ctrl(nullptr)
	{
		
	}
	
	template <class T>
	constexpr weak_ptr<T>::weak_ptr(nullptr_t n) : ctrl(n)
	{
		
	}
	
	template <class T>
	template <class Y>
	weak_ptr<T>::weak_ptr(const shared_ptr<Y>& r) : ctrl(r.ctrl)
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
	void weak_ptr<T>::reset()
	{
		if (ctrl)
		{
			if (!ctrl->weakRelease())
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
	}
	
	template <class T>
	long weak_ptr<T>::use_count() const noexcept
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
	bool weak_ptr<T>::expired() const
	{
		if (ctrl)
		{
			return ctrl->usecount == 0;
		}
		return true;
	}
	
	template <class T>
	shared_ptr<T> weak_ptr<T>::lock() const
	{
		return expired() ? shared_ptr<T>(nullptr) : shared_ptr<T>(ctrl);
	}
	
	
	
	
	
	
	
	template <class T>
	weak_ptr<T>& weak_ptr<T>::operator=(const weak_ptr& r)
	{
		auto ctrl2 = r.ctrl;
		if (ctrl2)
		{
			++ctrl2->refcount;
		}
		reset();
		ctrl = ctrl2;
		return *this;
	}
	
}

#endif