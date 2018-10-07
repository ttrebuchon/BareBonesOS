#ifndef INCLUDED_SHARED_PTR_HH
#define INCLUDED_SHARED_PTR_HH

#include <Common.h>
#include <Utils/detail/type_traits.hh>
#include <Utils/detail/enable_if.hh>

namespace Utils
{
	namespace detail
	{
		class shared_ptr_control;
		
	}
	
	template <class T>
	class weak_ptr;
	
	template <class T>
	class enable_shared_from_this;
	
	
	template <class T>
	class shared_ptr
	{
		public:
		// Member Types
		typedef T element_type;
		typedef T* ptr_type;
		typedef T& ref_type;
		
		private:
		
		// Members
		mutable detail::shared_ptr_control* ctrl;
		mutable element_type* other;
		
		//Private Constructors
		template <class Deleter>
		shared_ptr(void*, size_t n, Deleter);
		shared_ptr(detail::shared_ptr_control*);
		
		shared_ptr(detail::shared_ptr_control*, element_type*);
		
		public:
		// Constructors / Destructors
		constexpr shared_ptr() noexcept;
		constexpr shared_ptr(nullptr_t) noexcept;
		template <class Y, typename = typename enable_if<is_convertible<T*, Y*>::value, void>::type>
		explicit shared_ptr(Y*);
		
		template <class Y, class Deleter>
		shared_ptr(Y* ptr, Deleter d);
		
		template <class Y, class Deleter, class Alloc>
		shared_ptr(Y* ptr, Deleter d, const Alloc&);
		
		template <class Y>
		shared_ptr(const shared_ptr<Y>&, element_type*);
		
		template <class Y, typename = typename enable_if<is_convertible<T*, Y*>::value, void>::type>
		shared_ptr(const shared_ptr<Y>&) noexcept;
		template <class Y, typename = typename enable_if<is_convertible<T*, Y*>::value, void>::type>
		shared_ptr(shared_ptr<Y>&&) noexcept;
		
		shared_ptr(const shared_ptr&) noexcept;
		shared_ptr(shared_ptr&&) noexcept;
		template <class Y>
		explicit shared_ptr(const weak_ptr<Y>&);
		
		
		~shared_ptr();
		
		// Methods
		ptr_type get() const noexcept;
		void reset();
		void swap(shared_ptr&) noexcept;
		long use_count() const noexcept;
		
		
		
		// Operators
		ref_type operator*() const noexcept;
		ptr_type operator->() const noexcept;
		operator bool() const noexcept;
		shared_ptr& operator=(const shared_ptr&) noexcept;
		ref_type operator[](ptrdiff_t) const;
		template <class Y>
		enable_if_t<is_convertible<T*, Y*>::value, shared_ptr&> operator=(const shared_ptr<Y>& r) noexcept
		{
			static_assert(Utils::is_convertible<T*, Y*>::value);
			
			auto ctrl2 = r.ctrl;
			if (ctrl2)
			{
				++ctrl2->refcount;
				++ctrl2->usecount;
			}
			reset();
			ctrl = ctrl2;
			return *this;
		}
		
		
		
		
		
		
		
		template <class>
		friend struct make_shared_t;
		
		template <class>
		friend class weak_ptr;
		
		template <class>
		friend class shared_ptr;
		
		template <class G, class Y>
		friend bool operator==(const shared_ptr<G>&, const shared_ptr<Y>&) noexcept;
		template <class G>
		friend bool operator==(const shared_ptr<G>&, nullptr_t) noexcept;
		template <class G, class Y>
		friend bool operator==(const shared_ptr<G>&, Y* const&) noexcept;
		
		
		template <class G, class Y>
		friend bool operator!=(const shared_ptr<G>&, const shared_ptr<Y>&) noexcept;
		template <class G>
		friend bool operator!=(const shared_ptr<G>&, nullptr_t) noexcept;
		template <class G, class Y>
		friend bool operator!=(const shared_ptr<G>&, Y* const&) noexcept;
		
		template <class>
		friend class enable_shared_from_this;
	};
	
	
	
	
	template <class T, class... Args>
	inline shared_ptr<T> make_shared(Args&&... args);
	
	
	
	
	
	
	template <class T, class Y>
	bool operator==(const shared_ptr<T>& l, const shared_ptr<Y>& r) noexcept
	{
		return l.get() == r.get();
	}
	
	template <class T>
	bool operator==(const shared_ptr<T>& l, nullptr_t n) noexcept
	{
		return l.get() == n;
	}
	
	template <class T, class Y>
	bool operator==(const shared_ptr<T>& l, Y* const& r) noexcept
	{
		return l.get() == r;
	}
	
	template <class T, class Y>
	bool operator!=(const shared_ptr<T>& l, const shared_ptr<Y>& r) noexcept
	{
		return l.get() != r.get();
	}
	
	template <class T>
	bool operator!=(const shared_ptr<T>& l, nullptr_t n) noexcept
	{
		return l.get() != n;
	}
	
	template <class T, class Y>
	bool operator!=(const shared_ptr<T>& l, Y* const& r) noexcept
	{
		return l.get() != r;
	}
	
	
	
	
	
	template <class T>
	shared_ptr<T> wrap_shared(T* ptr)
	{
		return shared_ptr<T>(ptr);
	}
	
	template <class T>
	shared_ptr<const T> wrap_shared(const T* ptr)
	{
		return shared_ptr<const T>(ptr);
	}
	
	
	
	
	
}
#endif