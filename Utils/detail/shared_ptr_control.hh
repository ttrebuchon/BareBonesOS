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
			typedef void(*Destructor_Ptr)(void*, void*);
			typedef void(*Deallocator)(void*, shared_ptr_control*);
			
			template <class Y, class Deleter>
			class Destructor
			{
				public:
				Deleter del;
				static void call(void* d, void* ptr)
				{
					Destructor* dest = static_cast<Destructor<Y, Deleter>*>(d);
					dest->del((Y*)ptr);
					delete dest;
				}
				
				template <class Alloc>
				static void call(void* data_ptr, void* ptr)
				{
					typedef alloc_deleter_t<Alloc> data_t;
					auto data = static_cast<data_t*>(data_ptr);
					Destructor* dest = static_cast<Destructor<Y, Deleter>*>(data->dest);
					dest->del((Y*)ptr);
					typename Alloc::template rebind<Destructor>::other dalloc(data->alloc);
					dalloc.destroy(dest);
					dalloc.deallocate(dest, 1);
					typename Alloc::template rebind<data_t>::other data_alloc(dalloc);
					data_alloc.destroy(data);
					data_alloc.deallocate(data, 1);
				}
				
				Destructor()
				{
					
				}
				
				Destructor(const Deleter& d) : del(d)
				{
					
				}
			};
			
			long refcount;
			long usecount;
			void* obj;
			void* deleter_obj;
			Destructor_Ptr deleter;
			void* dealloc_object;
			Deallocator dealloc;
			
			
			shared_ptr_control();
			
			bool strongRelease();
			bool weakRelease();
			
			
			template <class Y, class Deleter, class Alloc = Utils::allocator<Y>>
			static detail::shared_ptr_control* CreateControl(Y*, Deleter, const Alloc& = Alloc());
			/*template <class Y, class Deleter>
			static detail::shared_ptr_control* CreateControl(Y*, Deleter);*/
			template <class Y>
			static detail::shared_ptr_control* CreateControl(Y*);
			
			/*template <class Y, class Deleter, class Alloc = Utils::Allocator<Y>>
			static detail::shared_ptr_control* InitControl(Y*, Deleter, const Alloc& = Alloc());*/
			template <class Y>
			static detail::shared_ptr_control* InitControl(detail::shared_ptr_control* ctrl, Y*);
			
		};
	}
}

#endif