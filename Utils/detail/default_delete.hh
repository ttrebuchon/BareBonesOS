#ifndef INCLUDED_DEFAULT_DELETE_HH
#define INCLUDED_DEFAULT_DELETE_HH

namespace Utils
{
	template <class T>
	struct default_delete
	{
		constexpr default_delete() noexcept = default;
		template <class U>
		default_delete(const default_delete<U>&) noexcept;
		
		__attribute__ ((__always_inline__)) void operator() (T* __ptr) const throw()
		{
			static_assert(sizeof(T) > 0, "Cannot delete incomplete type");
			delete __ptr;
		}
		
		private:
		
	};
	
	template <class T>
	struct default_delete<T[]>
	{
		constexpr default_delete() noexcept = default;
		template <class U>
		default_delete(const default_delete<U[]>&) noexcept;
		
		template <class U>
		void operator()(U* ptr) const
		{
			call(ptr);
		}
		
		private:
		void call(T* t) const
		{
			static_assert(sizeof(T) > 0, "Cannot delete incomplete type");
			delete[] t;
		}
	};
}

#endif