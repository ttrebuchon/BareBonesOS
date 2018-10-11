#ifndef INCLUDED_SHARED_PTR_CONTROL_HPP
#define INCLUDED_SHARED_PTR_CONTROL_HPP

#include "shared_ptr_control.hh"
#include <Utils/Allocator_Traits.hh>

namespace Utils
{
namespace detail
{
	template <class Y, class Deleter, class Alloc>
	struct shared_ptr_control_deleter_info
	{
		Y* ptr;
		Deleter del;
		Alloc allocator;
		
		shared_ptr_control_deleter_info(Y* ptr, const Deleter& d, const Alloc& a) : ptr(ptr), del(d), allocator(allocator_traits<Alloc>::copy_create(a))
		{
			
		}
	};
	
	
	
	
	
	
	
	
	template <class Y, class Deleter, class Alloc>
	shared_ptr_control* shared_ptr_control::CreateControl(Y* ptr, Deleter d, const Alloc& _alloc)
	{
		typedef typename Alloc::template rebind<shared_ptr_control>::other CtrlAlloc;
		typedef shared_ptr_control_deleter_info<Y, Deleter, Alloc> info_type;
		
		auto info_alloc = allocator_traits<typename Alloc::template rebind<info_type>::other>::copy_create(_alloc);
		auto info = info_alloc.allocate(1);
		new (info) info_type(ptr, d, _alloc);
		
		auto ctrl_alloc = allocator_traits<CtrlAlloc>::copy_create(_alloc);
		auto ctrl = ctrl_alloc.allocate(1);
		new (ctrl) shared_ptr_control();
		InitControl(ctrl);
		ctrl->data = info;
		
		ctrl->deleter = [](void* data)
		{
			
			auto info = (info_type*)data;
			(info->del)(info->ptr);
			info->ptr = nullptr;
		};
		
		
		ctrl->deallocator = [](shared_ptr_control* ctrl)
		{
			auto info = (info_type*)ctrl->data;
			
			CtrlAlloc ctrl_alloc = allocator_traits<CtrlAlloc>::copy_create(info->allocator);
			
			auto info_alloc = allocator_traits<typename Alloc::template rebind<info_type>::other>::copy_create(info->allocator);
			
			info->~info_type();
			info_alloc.deallocate(info, 1);
			
			ctrl->~shared_ptr_control();
			ctrl_alloc.deallocate(ctrl, 1);
		};
		return ctrl;
	}
	
	
	template <class Y>
	shared_ptr_control* shared_ptr_control::CreateControl(Y* ptr)
	{
		auto ctrl = new shared_ptr_control();
		InitControl(ctrl);
		ctrl->data = ptr;
		ctrl->deleter = [](void* data)
		{
			delete (Y*)data;
		};
		return ctrl;
	}
	
}
}

#endif