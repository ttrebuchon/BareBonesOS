#ifndef INCLUDED_SHARED_PTR_CONTROL_HPP
#define INCLUDED_SHARED_PTR_CONTROL_HPP

#include "shared_ptr_control.hh"

namespace Utils
{
namespace detail
{
	shared_ptr_control::shared_ptr_control() : refcount(0), usecount(0), obj(nullptr), deleter_obj(nullptr), deleter(nullptr), dealloc_object(nullptr), dealloc(nullptr)
	{
		
	}
	
	bool shared_ptr_control::strongRelease()
	{
		if (--usecount == 0)
		{
			deleter(deleter_obj, obj);
			obj = nullptr;
		}
		
		return (--refcount != 0);
	}
	
	bool shared_ptr_control::weakRelease()
	{
		return (--refcount != 0);
	}
	
	
	
	
	
	
	
	
	
	
	template <class Y, class Deleter, class Alloc>
	shared_ptr_control* shared_ptr_control::CreateControl(Y* ptr, Deleter d, Alloc _alloc)
	{
		auto calloc = new typename Alloc::template rebind<shared_ptr_control>::other();
		typename Alloc::template rebind<shared_ptr_control::template Destructor<Y, Deleter>>::other dalloc;
		auto ctrl = calloc->construct(calloc->allocate(1));
		ctrl->dealloc_object = calloc;
		ctrl->dealloc = [](void* aptr, shared_ptr_control* ctrl)
		{
			auto calloc = (typename Alloc::template rebind<shared_ptr_control>::other*)aptr;
			calloc->destroy(ctrl);
			calloc->deallocate(ctrl);
			delete calloc;
		};
		
		ctrl->refcount = ctrl->usecount = 1;
		auto destr = dalloc.construct(dalloc.allocate(1));
		destr->del = d;
		ctrl->deleter_obj = (void*)destr;
		ctrl->deleter = shared_ptr_control::template Destructor<Y, Deleter>::template call<Alloc>;
		ctrl->obj = ptr;
		return ctrl;
	}
	
	template <class Y, class Deleter>
	shared_ptr_control* shared_ptr_control::CreateControl(Y* ptr, Deleter d)
	{
		auto ctrl = new shared_ptr_control();
		ctrl->refcount = ctrl->usecount = 1;
		auto destr = new typename shared_ptr_control::template Destructor<Y, Deleter>();
		destr->del = d;
		ctrl->deleter_obj = (void*)destr;
		ctrl->deleter = shared_ptr_control::template Destructor<Y, Deleter>::call;
		ctrl->obj = ptr;
		return ctrl;
	}
	
	template <class Y>
	shared_ptr_control* shared_ptr_control::CreateControl(Y* ptr)
	{
		auto ctrl = new shared_ptr_control();
		ctrl->refcount = ctrl->usecount = 1;
		ctrl->deleter = [](void*, void* tptr)
		{
			delete (Y*)tptr;
		};
		ctrl->obj = ptr;
		return ctrl;
	}
}
}

#endif