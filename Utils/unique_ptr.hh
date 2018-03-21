#ifndef INCLUDED_UNIQUE_PTR_HH
#define INCLUDED_UNIQUE_PTR_HH

#include <Common.h>
#include "detail/default_delete.hh"
#include "type_traits"
#include "void_t.hh"

namespace Utils
{
	namespace detail
	{
		class shared_ptr_control;
		
		template <class D, class T, class = void>
		struct unique_ptr_get_type
		{
			typedef T* type;
		};
		
		template <class D, class T>
		struct unique_ptr_get_type<D, T, void_t<typename D::pointer>>
		{
			typedef typename D::pointer type;
		};
		
		template <class A>
		struct unique_ptr_deleter_args
		{
			typedef const A& type1;
			typedef A&& type2;
		};
		
		template <class A>
		struct unique_ptr_deleter_args<A&>
		{
			typedef A& type1;
			typedef A&& type2;
		};
		
		template <class A>
		struct unique_ptr_deleter_args<const A&>
		{
			typedef const A& type1;
			typedef const A&& type2;
		};
		
	}
	
	
	template <class T, class Deleter = default_delete<T>>
	class unique_ptr
	{
		public:
		// Member Types
		typedef typename detail::unique_ptr_get_type<typename remove_reference<Deleter>::type, T>::type pointer;
		typedef T& ref_type;
		typedef T element_type;
		typedef Deleter deleter_type;
		
		private:
		pointer obj;
		Deleter del;
		
		public:
		
		// Constructors / Destructors
		constexpr unique_ptr() noexcept;
		constexpr unique_ptr(nullptr_t) noexcept;
		explicit unique_ptr(pointer);
		unique_ptr(pointer, typename detail::unique_ptr_deleter_args<Deleter>::type1) noexcept;
		unique_ptr(pointer, typename detail::unique_ptr_deleter_args<Deleter>::type2) noexcept;
		template <class U, class E>
		unique_ptr(unique_ptr<U, E>&&) noexcept;
		unique_ptr(unique_ptr&&) noexcept;
		
		~unique_ptr() noexcept;
		
		// Methods
		pointer release() noexcept;
		void reset() noexcept;
		void swap(unique_ptr&) noexcept;
		pointer get() const noexcept;
		Deleter& get_deleter() noexcept;
		const Deleter& get_deleter() const noexcept;
		
		
		// Operators
		ref_type operator*() const noexcept;
		pointer operator->() const noexcept;
		operator bool() const noexcept;
		unique_ptr& operator=(unique_ptr&&) noexcept;
		unique_ptr& operator=(nullptr_t) noexcept;
		ref_type operator[](ptrdiff_t) const;
		template <class U, class E>
		unique_ptr& operator=(unique_ptr<U, E>&&) noexcept;
		
		
		
		
		
		
		
		template <class>
		friend struct make_unique_t;
		
		/*template <class G, class Y>
		friend bool operator==(const unique_ptr<G>&, const unique_ptr<Y>&) noexcept;
		template <class G>
		friend bool operator==(const unique_ptr<G>&, nullptr_t) noexcept;
		template <class G, class Y>
		friend bool operator==(const unique_ptr<G>&, Y* const&) noexcept;
		
		
		template <class G, class Y>
		friend bool operator!=(const unique_ptr<G>&, const unique_ptr<Y>&) noexcept;
		template <class G>
		friend bool operator!=(const unique_ptr<G>&, nullptr_t) noexcept;
		template <class G, class Y>
		friend bool operator!=(const unique_ptr<G>&, Y* const&) noexcept;*/
	};
	
	
	
	
	
	
	
	template <class T, class... Args>
	inline unique_ptr<T> make_unique(Args&&... args);
	
	
	
	
	
	
	template <class T, class D1, class Y, class D2>
	bool operator==(const unique_ptr<T, D1>& l, const unique_ptr<Y, D2>& r) noexcept
	{
		return l.get() == r.get();
	}
	
	template <class T, class D>
	bool operator==(const unique_ptr<T, D>& l, nullptr_t n) noexcept
	{
		return l.get() == n;
	}
	
	template <class T, class D1, class Y>
	bool operator==(const unique_ptr<T, D1>& l, Y* const& r) noexcept
	{
		return l.get() == r;
	}
	
	template <class T, class D1, class Y, class D2>
	bool operator!=(const unique_ptr<T, D1>& l, const unique_ptr<Y, D2>& r) noexcept
	{
		return l.get() != r.get();
	}
	
	template <class T, class D>
	bool operator!=(const unique_ptr<T, D>& l, nullptr_t n) noexcept
	{
		return l.get() != n;
	}
	
	template <class T, class D1, class Y>
	bool operator!=(const unique_ptr<T, D1>& l, Y* const& r) noexcept
	{
		return l.get() != r;
	}
	
	
}
#endif