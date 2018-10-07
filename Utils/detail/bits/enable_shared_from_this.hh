#ifndef INCLUDED_BITS_ENABLE_SHARED_FROM_THIS_HH
#define INCLUDED_BITS_ENABLE_SHARED_FROM_THIS_HH

#include <Utils/shared_ptr.hh>
#include <Utils/weak_ptr.hh>

namespace Utils
{
	namespace detail
	{
		template <class, class, bool>
		struct use_SFT_helper;
	}
	
	template <class T>
	class enable_shared_from_this
	{
		private:
		mutable weak_ptr<T> refer;
		
		protected:
		constexpr enable_shared_from_this() : refer()
		{
			
		}
		
		enable_shared_from_this(const enable_shared_from_this& o) : refer()
		{
			
		}
		
		enable_shared_from_this& operator=(const enable_shared_from_this&)
		{ return *this; }
		
		~enable_shared_from_this()
		{
			
		}
		
		public:
		
		
		shared_ptr<T> shared_from_this()
		{
			return Utils::shared_ptr<T>(refer);
			/*if (ctrl)
			{
				return shared_ptr<T>(ctrl, ptr);
			}
			else
			{
				// TODO: Throw or abort
				assert(NOT_IMPLEMENTED);
			}*/
		}
		
		shared_ptr<const T> shared_from_this() const
		{
			return Utils::shared_ptr<T>(refer);
			/*if (ctrl)
			{
				return shared_ptr<T>(ctrl, ptr);
			}
			else
			{
				// TODO: Throw or abort
				assert(NOT_IMPLEMENTED);
			}*/
		}
		
		private:
		
		template <class T1>
		void weak_assign(detail::shared_ptr_control* ctrl, T1* ptr)
		{
			refer.assign(ctrl, ptr);
		}
		
		
		
		template <class>
		friend class shared_ptr;
		
		template <class, class, bool>
		friend struct detail::use_SFT_helper;
	};
	
}

#endif