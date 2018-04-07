#ifndef INCLUDED_BITS_FUNCTION_IMPL_HH
#define INCLUDED_BITS_FUNCTION_IMPL_HH

#include <Common.h>
#include <Utils/utility>
#include <Utils/memory>
#include <Utils/typeinfo>

namespace Utils { namespace detail
{
	template <class...>
	class func_impl;
	
	template <class Ret, class... Args>
	class func_impl<Ret(Args...)>
	{
		protected:
		
		public:
		
		virtual ~func_impl() = default;
		
		virtual Ret invoke(Args...) = 0;
		virtual func_impl* clone() const = 0;
		
		#ifdef __cpp_rtti
		virtual void* target(const type_info&) = 0;
		virtual const void* target(const type_info&) const = 0;
		virtual const type_info& info() const noexcept = 0;
		#endif
		
		template <class _Fn, class Alloc = Utils::allocator<void>>
		static func_impl* Make(_Fn f, const Alloc& alloc = Alloc());
		
		/*template <class Fn>
		static func_impl* Make(Fn f)
		{
			auto ptr = new func_impl();
			auto con = new Container<Fn>(forward<Fn>(f));
			ptr->obj = con;
			ptr->invoker = Container<Fn>::invoke;
			ptr->destroyer = Container<Fn>::destroy;
			
			return ptr->control();
		}*/
		
	};
	
	
	template <class Fn, class Ret, class... Args>
	class func_impl_fn;
	
	 template <class Fn, class Ret, class... Args>
	class func_impl_fn<Fn, Ret(Args...)> : public func_impl<Ret(Args...)>
	{
		protected:
		typedef func_impl<Ret(Args...)> _Base;
		Fn fn;
		
		
		public:
		func_impl_fn(const Fn& f) : _Base(), fn(f)
		{
			
		}
		
		func_impl_fn(Fn&& f) : _Base(), fn(forward<Fn&&>(f))
		{
			
		}
		
		virtual ~func_impl_fn() = default;
		
		
		virtual Ret invoke(Args... args) override
		{
			return fn(forward<Args>(args)...);
		}
		
		
		virtual func_impl_fn* clone() const override
		{
			return new func_impl_fn(fn);
		}
		
		#ifdef __cpp_rtti
		virtual void* target(const type_info& info) override
		{
			if (info == typeid(Fn))
			{
				return &fn;
			}
			return nullptr;
		}
		
		virtual const void* target(const type_info& info) const override
		{
			if (info == typeid(Fn))
			{
				return &fn;
			}
			return nullptr;
		}
		
		virtual const type_info& info() const noexcept override
		{
			return typeid(fn);
		}
		#endif
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	template <class Ret, class... Args>
	template <class _Fn, class Alloc>
	auto func_impl<Ret(Args...)>::Make(_Fn f, const Alloc& alloc) -> func_impl*
	{
		typedef typename remove_cv<typename remove_reference<_Fn>::type>::type Fn;
		typedef func_impl_fn<Fn, Ret(Args...)> Imp_t;
			
		typename Alloc::template rebind<Imp_t>::other a(alloc);
		auto ptr = a.allocate(1);
		a.construct(ptr, forward<_Fn>(f));
		
		return ptr;
	}
}
}
#endif