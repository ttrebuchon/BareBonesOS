#ifndef INCLUDED_SHARED_PTR_CONTROL_HPP
#define INCLUDED_SHARED_PTR_CONTROL_HPP

#include "shared_ptr_control.hh"
#include <Utils/Allocator_Traits.hh>

namespace Utils
{
namespace detail
{
	
	
	
	
	
	
	
	
	
	template <class Y, class Deleter, class Alloc>
	shared_ptr_control* shared_ptr_control::CreateControl(Y* ptr, Deleter d, const Alloc& _alloc)
	{
		typedef typename Alloc::template rebind<shared_ptr_control>::other CtrlAlloc;
		
		auto calloc_alloc = allocator_traits<typename Alloc::template rebind<CtrlAlloc>::other>::copy_create(_alloc);
		
		CtrlAlloc* calloc = calloc_alloc.allocate(1);
		allocator_traits<CtrlAlloc>::copy_create(calloc_alloc, calloc, _alloc);
		
		
		auto dalloc = allocator_traits<typename Alloc::template rebind<shared_ptr_control::template Destructor<Y, Deleter>>::other>::copy_create(_alloc);
		auto ctrl = calloc->allocate(1);
		calloc->construct(ctrl);
		ctrl->dealloc_object = calloc;
		ctrl->dealloc = [](void* aptr, shared_ptr_control* ctrl)
		{
			auto calloc = (CtrlAlloc*)aptr;
			calloc->destroy(ctrl);
			calloc->deallocate(ctrl, 1);
			
			auto calloc_alloc = allocator_traits<typename Alloc::template rebind<CtrlAlloc>::other>::copy_create(*calloc);
			calloc_alloc.destroy(calloc);
			calloc_alloc.deallocate(calloc, 1);
			
		};
		
		ctrl->refcount = ctrl->usecount = 1;
		auto destr = dalloc.allocate(1);
		dalloc.construct(destr, d);
		//destr->del = d;
		
		typedef typename Alloc::template rebind<alloc_deleter_t<Alloc>>::other data_alloc_t;
		data_alloc_t data_alloc(_alloc);
		auto data = data_alloc.allocate(1);
		data_alloc.construct(data, _alloc);
		data->dest = (void*)destr;
		
		ctrl->deleter_obj = (void*)data;
		ctrl->deleter = shared_ptr_control::template Destructor<Y, Deleter>::template call<Alloc>;
		ctrl->obj = ptr;
		return ctrl;
	}
	
	
	
	
	/*template <class Y, class Deleter>
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
	}*/
	
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