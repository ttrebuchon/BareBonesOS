#ifndef INCLUDED_BITS_FUNCTION_HPP
#define INCLUDED_BITS_FUNCTION_HPP

#include "function.hh"

namespace Utils
{
	template <class Ret, class... Args>
	function<Ret(Args...)>::function() noexcept : _imp(nullptr)
	{
		
	}
	
	template <class Ret, class... Args>
	function<Ret(Args...)>::function(nullptr_t) noexcept : _imp(nullptr)
	{
		
	}
	
	
	template <class Ret, class... Args>
	template <class Fn>
	function<Ret(Args...)>::function(Fn f) : _imp(Impl_t::Make(forward<Fn>(f)))
	{
		
	}
	
	template <class Ret, class... Args>
	function<Ret(Args...)>::function(const function& f) : _imp(f._imp->clone())
	{
		
	}
	
	template <class Ret, class... Args>
	function<Ret(Args...)>::function(function&& f) noexcept : _imp()
	{
		_imp.swap(f._imp);
	}
	
	template <class Ret, class... Args>
	template <class Alloc>
	function<Ret(Args...)>::function(allocator_arg_t, const Alloc&) noexcept : _imp(nullptr)
	{
		
	}
	
	template <class Ret, class... Args>
	template <class Alloc>
	function<Ret(Args...)>::function(allocator_arg_t, const Alloc&, nullptr_t) noexcept : _imp(nullptr)
	{
		
	}
	
	template <class Ret, class... Args>
	template <class Fn, class Alloc>
	function<Ret(Args...)>::function(allocator_arg_t, const Alloc& a, Fn f) : _imp(Impl_t::Make(forward<Fn>(f), a))
	{
		
	}
	
	
	
	
	
	
	// Methods
	
	template <class Ret, class... Args>
	template <class Fn, class Alloc>
	void function<Ret(Args...)>::assign(Fn&& f, const Alloc& a)
	{
		_imp.reset();
		_imp = unique_ptr<Impl_t>(Impl_t::Make(forward<Fn&&>(f), a));
	}
	
	#ifdef __cpp_rtti
	template <class Ret, class... Args>
	const type_info& function<Ret(Args...)>::target_type() const noexcept
	{
		return _imp->info();
	}
	
	template <class Ret, class... Args>
	template <class TargetType>
	TargetType* function<Ret(Args...)>::target() noexcept
	{
		return (TargetType*)_imp->target(typeid(TargetType));
	}
	
	template <class Ret, class... Args>
	template <class TargetType>
	const TargetType* function<Ret(Args...)>::target() const noexcept
	{
		return (const TargetType*)_imp->target(typeid(TargetType));
	}
	#endif
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// Operators
	
	
	template <class Ret, class... Args>
	function<Ret(Args...)>& function<Ret(Args...)>::operator=(const function& other)
	{
		_imp = other._imp->clone();
		return *this;
	}
	
	template <class Ret, class... Args>
	function<Ret(Args...)>& function<Ret(Args...)>::operator=(function&& other)
	{
		_imp = Utils::move(other._imp);
		return *this;
	}
	
	template <class Ret, class... Args>
	template <class Fn>
	typename enable_if<is_same<decltype(declval<Fn>()(declval<Args>()...)), Ret>::value, function<Ret(Args...)>&>::type function<Ret(Args...)>::operator=(Fn&& other)
	{
		_imp = unique_ptr<Impl_t>(Impl_t::Make(forward<Fn&&>(other)));
		return *this;
	}
	
	template <class Ret, class... Args>
	function<Ret(Args...)>::operator bool() const noexcept
	{
		return _imp != nullptr;
	}
	
	template <class Ret, class... Args>
	Ret function<Ret(Args...)>::operator()(Args... args) const
	{
		return _imp->invoke(forward<Args>(args)...);
	}
}

#endif