#ifndef INCLUDED_BITS_FUNCTION_HH
#define INCLUDED_BITS_FUNCTION_HH

#include <Common.h>
#include "allocator_arg_t.hh"
#include "reference_wrapper.hh"
#include "function_impl.hh"
#include <Utils/memory>

namespace Utils
{
	
	
	template <class T>
	class function;
	
	namespace detail
	{
		template <class...>
		struct function_typedefs
		{};
		
		template <class Arg>
		struct function_typedefs<Arg>
		{
			typedef Arg argument_type;
		};
		
		template <class Arg1, class Arg2>
		struct function_typedefs<Arg1, Arg2>
		{
			typedef Arg1 first_argument_type;
			typedef Arg2 second_argument_type;
		};
	}
	
	
	template <class Ret, class... Args>
	class function<Ret(Args...)> : public detail::function_typedefs<Args...>
	{
		public:
		typedef Ret result_type;
		
		
		protected:
		typedef detail::func_impl<Ret(Args...)> Impl_t;
		Utils::unique_ptr<Impl_t> _imp;
		
		
		public:
		
		// Constructors
		function() noexcept;
		function(nullptr_t) noexcept;
		template <class Fn>
		function(Fn);
		function(const function&);
		function(function&&) noexcept;
		
		// Allocator-Constructors
		template <class Alloc>
		function(allocator_arg_t aa, const Alloc&) noexcept;
		template <class Alloc>
		function(allocator_arg_t aa, const Alloc&, nullptr_t) noexcept;
		template <class Fn, class Alloc>
		function(allocator_arg_t aa, const Alloc&, Fn);
		template <class Alloc>
		function(allocator_arg_t aa, const Alloc&, const function&);
		template <class Alloc>
		function(allocator_arg_t aa, const Alloc&, function&&);
		
		// Destructor
		~function() = default;
		
		
		// Methods
		template <class Fn, class Alloc>
		void assign(Fn&&, const Alloc&);
		#ifdef __cpp_rtti
		const type_info& target_type() const noexcept;
		template <class TargetType>
		TargetType* target() noexcept;
		template <class TargetType>
		const TargetType* target() const noexcept;
		#endif
		void swap(function&) noexcept;
		
		
		// Operators
		function& operator=(const function&);
		function& operator=(function&&);
		template <class Fn>
		function& operator=(Fn&&);
		template <class Fn>
		function& operator=(reference_wrapper<Fn>) noexcept;
		function& operator=(nullptr_t);
		explicit operator bool() const noexcept;
		Ret operator()(Args...) const;
	};
}

#endif