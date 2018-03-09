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
		
		void operator()(T* ptr) const
		{
			delete ptr;
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
			delete[] t;
		}
	};
}

#endif