#ifndef INCLUDED_SHARED_PTR_CONTROL_HH
#define INCLUDED_SHARED_PTR_CONTROL_HH

#include <Common.h>
#include <Utils/Allocator.hh>

namespace Utils
{
	namespace detail
	{
		template <class Alloc>
		struct alloc_deleter_t
		{
			void* dest;
			Alloc alloc;
			
			alloc_deleter_t(const Alloc& a) : alloc(a)
			{}
		};
		
		class shared_ptr_control
		{
			public:
			typedef void(*Destructor_Ptr)(void*);
			typedef void(*Deallocator_Ptr)(shared_ptr_control*);
			
			long refcount;
			long usecount;
			void* data;
			Destructor_Ptr deleter;
			Deallocator_Ptr deallocator;
			
			
			shared_ptr_control();
			
			bool strongRelease();
			bool weakRelease();
			
			
			template <class Y, class Deleter, class Alloc = Utils::allocator<Y>>
			static detail::shared_ptr_control* CreateControl(Y*, Deleter, const Alloc& = Alloc());
			template <class Y>
			static detail::shared_ptr_control* CreateControl(Y*);
			
			static detail::shared_ptr_control* InitControl(detail::shared_ptr_control* ctrl)
			{
				assert(ctrl);
				ctrl->refcount = ctrl->usecount = 1;
				return ctrl;
			}
			
			
			static void Release(shared_ptr_control* ctrl)
			{
				if (ctrl->deallocator)
				{
					ctrl->deallocator(ctrl);
				}
				else
				{
					delete ctrl;
				}
			}
			
		};
	}
}

#endif