#ifndef INCLUDED_BITS_ENABLE_SHARED_FROM_THIS_HH
#define INCLUDED_BITS_ENABLE_SHARED_FROM_THIS_HH

#include <Utils/shared_ptr.hh>
#include <Utils/weak_ptr.hh>

namespace Utils
{
	
	template <class T>
	class enable_shared_from_this
	{
		private:
		mutable weak_ptr<T> weak_this;
		
		protected:
		constexpr enable_shared_from_this()
		{
			
		}
		
		enable_shared_from_this(const enable_shared_from_this&)
		{}
		
		enable_shared_from_this& operator=(const enable_shared_from_this&)
		{ return *this; }
		
		~enable_shared_from_this() {}
		
		public:
		
		
		shared_ptr<T> shared_from_this()
		{
			return shared_ptr<T>(this->weak_this);
		}
		
		shared_ptr<const T> shared_from_this() const
		{
			return shared_ptr<const T>(this->weak_this);
		}
		
		private:
		
		template <class T1>
		void weak_assign(const detail::shared_ptr_control* ctrl)
		{
			weak_this.assign(ctrl);
		}
		
		
		template <class T1>
		friend void __enable_shared_from_this_helper(const detail::shared_ptr_control* ctrl, const enable_shared_from_this* sft, const T1* ptr)
		{
			if (sft)
			{
				sft->weak_assign(/*const_cast<T1*>(ptr),*/ ctrl);
			}
		}
	};
	
}

#endif