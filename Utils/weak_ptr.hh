#ifndef INCLUDED_WEAK_PTR_HH
#define INCLUDED_WEAK_PTR_HH

#include <Common.h>
#include "shared_ptr.hh"

namespace Utils
{
	namespace detail
	{
		template <class, class, bool>
		struct use_SFT_helper;
		class shared_ptr_control;
	}
	
	
	template <class T>
	class shared_ptr;
	
	template <class T>
	class weak_ptr
	{
		public:
		// Member Types
		typedef T element_type;
		typedef T* ptr_type;
		typedef T& ref_type;
		
		
		private:
		
		// Members
		mutable detail::shared_ptr_control* ctrl;
		T* ptr;
		
		void assign(detail::shared_ptr_control*, T* ptr);
		
		
		public:
		constexpr weak_ptr() noexcept;
		constexpr weak_ptr(nullptr_t) noexcept;
		weak_ptr(const weak_ptr&) noexcept;
		template <class Y>
		weak_ptr(const shared_ptr<Y>&) noexcept;
		template <class Y>
		weak_ptr(const weak_ptr<Y>&) noexcept;
		weak_ptr(weak_ptr&&) noexcept;
		template <class Y>
		weak_ptr(weak_ptr<Y>&&) noexcept;
		
		~weak_ptr();
		
		// Methods
		void reset();
		void swap(weak_ptr&) noexcept;
		long use_count() const noexcept;
		bool expired() const noexcept;
		shared_ptr<T> lock() const noexcept;
		
		
		// Operators
		weak_ptr& operator=(const weak_ptr&);
		
		
		template <class>
		friend class shared_ptr;
		
		template <class>
		friend class enable_shared_from_this;
		
		template <class, class, bool>
		friend struct detail::use_SFT_helper;
	};
	
}

#endif