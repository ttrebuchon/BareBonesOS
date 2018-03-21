#ifndef INCLUDED_SHARED_PTR_CONTROL_HH
#define INCLUDED_SHARED_PTR_CONTROL_HH

namespace Utils
{
	namespace detail
	{
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
				static void call(void* d, void* ptr)
				{
					Destructor* dest = static_cast<Destructor<Y, Deleter>*>(d);
					dest->del((Y*)ptr);
					typename Alloc::template rebind<Destructor>::other dalloc;
					dalloc.destroy(dest);
					dalloc.deallocate(dest);
				}
				
				Destructor()
				{
					
				}
				
				Destructor(Deleter d) : del(d)
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
			
			
			template <class Y, class Deleter, class Alloc>
			static detail::shared_ptr_control* CreateControl(Y*, Deleter, Alloc);
			template <class Y, class Deleter>
			static detail::shared_ptr_control* CreateControl(Y*, Deleter);
			template <class Y>
			static detail::shared_ptr_control* CreateControl(Y*);
			
		};
	}
}

#endif