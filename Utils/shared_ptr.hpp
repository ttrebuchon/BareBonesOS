#ifndef INCLUDED_SHARED_PTR_HPP
#define INCLUDED_SHARED_PTR_HPP

#include "shared_ptr.hh"
#include "detail/forward.hh"
//#include <new>

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
		}
		
		return (--refcount != 0);
	}
}
	
	
	
	
	
	
	
	
	
	
	
	
	template <class T>
	constexpr shared_ptr<T>::shared_ptr() : ctrl(nullptr)
	{
		
	}
	
	template <class T>
	constexpr shared_ptr<T>::shared_ptr(nullptr_t n) : ctrl(n)
	{
		
	}
	
	template <class T>
	template <class Y, class Deleter>
	shared_ptr<T>::shared_ptr(Y* ptr, Deleter d) : ctrl(createControl(ptr, d))
	{
		
	}
	
	template <class T>
	template <class Y, class Deleter, class Alloc>
	shared_ptr<T>::shared_ptr(Y* ptr, Deleter d, Alloc alloc) : ctrl(createControl(ptr, d, alloc))
	{
		
	}
	
	template <class T>
	template <class Y>
	shared_ptr<T>::shared_ptr(Y* ptr) : ctrl(createControl(ptr))
	{
		
	}
	
	template <class T>
	template <class Y>
	shared_ptr<T>::shared_ptr(const shared_ptr<Y>& r) : ctrl(r.ctrl)
	{
		++ctrl->refcount;
		++ctrl->usecount;
	}
	
	template <class T>
	shared_ptr<T>::shared_ptr(const shared_ptr& r) : ctrl(r.ctrl)
	{
		++ctrl->refcount;
		++ctrl->usecount;
	}
	
	template <class T>
	template <class Deleter>
	shared_ptr<T>::shared_ptr(void* raw, size_t nbytes, Deleter d) : ctrl(nullptr)
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
	shared_ptr<T>::~shared_ptr()
	{
		reset();
	}
	
	
	
	
	
	
	
	template <class T>
	template <class Y, class Deleter, class Alloc>
	detail::shared_ptr_control* shared_ptr<T>::createControl(Y* ptr, Deleter d, Alloc _alloc)
	{
		auto calloc = new typename Alloc::template rebind<detail::shared_ptr_control>::other();
		typename Alloc::template rebind<detail::shared_ptr_control::template Destructor<Y, Deleter>>::other dalloc;
		auto ctrl = calloc->construct(calloc->allocate(1));
		ctrl->dealloc_object = calloc;
		ctrl->dealloc = [](void* aptr, detail::shared_ptr_control* ctrl)
		{
			auto calloc = (typename Alloc::template rebind<detail::shared_ptr_control>::other*)aptr;
			calloc->destroy(ctrl);
			calloc->deallocate(ctrl);
			delete calloc;
		};
		
		ctrl->refcount = ctrl->usecount = 1;
		auto destr = dalloc.construct(dalloc.allocate(1));
		destr->del = d;
		ctrl->deleter_obj = (void*)destr;
		ctrl->deleter = detail::shared_ptr_control::template Destructor<Y, Deleter>::template call<Alloc>;
		ctrl->obj = (T*)ptr;
		return ctrl;
	}
	
	template <class T>
	template <class Y, class Deleter>
	detail::shared_ptr_control* shared_ptr<T>::createControl(Y* ptr, Deleter d)
	{
		auto ctrl = new detail::shared_ptr_control();
		ctrl->refcount = ctrl->usecount = 1;
		auto destr = new typename detail::shared_ptr_control::template Destructor<Y, Deleter>();
		destr->del = d;
		ctrl->deleter_obj = (void*)destr;
		ctrl->deleter = detail::shared_ptr_control::template Destructor<Y, Deleter>::call;
		ctrl->obj = (T*)ptr;
		return ctrl;
	}
	
	template <class T>
	template <class Y>
	detail::shared_ptr_control* shared_ptr<T>::createControl(Y* ptr)
	{
		auto ctrl = new detail::shared_ptr_control();
		ctrl->refcount = ctrl->usecount = 1;
		ctrl->deleter = [](void*, void* tptr)
		{
			delete (Y*)tptr;
		};
		ctrl->obj = (T*)ptr;
		return ctrl;
	}
	
	
	
	template <class T>
	typename shared_ptr<T>::ptr_type shared_ptr<T>::get() const
	{
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
	shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr& r)
	{
		auto ctrl2 = r.ctrl;
		++ctrl2->refcount;
		++ctrl2->usecount;
		reset();
		ctrl = ctrl2;
		return *this;
	}
	
	template <class T>
	shared_ptr<T>::operator bool() const noexcept
	{
		return get() != nullptr;
	}
	
	template <class T>
	typename shared_ptr<T>::ref_type shared_ptr<T>::operator*() const
	{
		return *(T*)ctrl->obj;
	}
	
	template <class T>
	typename shared_ptr<T>::ptr_type shared_ptr<T>::operator->() const
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
			
			
			void* raw = ::operator new ((sizeof(T) + sizeof(detail::shared_ptr_control::Destructor<T, DelPtr>) + sizeof(detail::shared_ptr_control))/sizeof(char));
			shared_ptr<T> ptr(raw,
				sizeof(T) + sizeof(detail::shared_ptr_control),
				(DelPtr)[](T* tptr)
				{
					delete tptr;
				}
				);
			new ((char*)raw +  (sizeof(detail::shared_ptr_control::Destructor<T, DelPtr>) + sizeof(detail::shared_ptr_control))/sizeof(char)) T(args...);
			return ptr;
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
		}
	};
	
	
	
	template <class T, class... Args>
	inline shared_ptr<T> make_shared(Args&&... args)
	{
		return make_shared_t<T>::make(forward<Args>(args)...);
	}
	
}

#endif