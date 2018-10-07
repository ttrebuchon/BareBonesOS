#ifndef INCLUDED_BITS_DETECT_SHARED_FROM_THIS_HH
#define INCLUDED_BITS_DETECT_SHARED_FROM_THIS_HH

#include "enable_shared_from_this.hh"
#include <Utils/utility>

namespace Utils::detail
{
	
	template <class T>
	struct has_shared_from_this_helper
	{
		static void* func(void*);
		static T* func(enable_shared_from_this<T>*);
	};
	
	template <class T, class Y>
	struct non_void_selector
	{
		typedef T type;
	};
	
	template <class T>
	struct non_void_selector<T, void*>
	{
		typedef T type;
	};
	
	template <class Y>
	struct non_void_selector<void*, Y>
	{
		typedef Y type;
	};
	
	template <>
	struct non_void_selector<void*, void*>
	{
		typedef void* type;
	};
	
	// Usage: shared_ptr<T>(Y*) { .... }
	template <class T, class Y = T>
	struct detect_shared_from_this
	{
		typedef decltype(has_shared_from_this_helper<T>::func(Utils::declval<Y*>())) type_1;
		
		typedef decltype(has_shared_from_this_helper<Y>::func(Utils::declval<Y*>())) type_2;
		
		constexpr static bool value_1 = !Utils::is_same<type_1, void*>::value;
		constexpr static bool value_2 = !Utils::is_same<type_2, void*>::value;
		
		
		
		typedef typename non_void_selector<type_1, type_2>::type type;
		
		constexpr static bool value = value_1 || value_2;
		
		static shared_ptr_control* use(Y*) noexcept;
		static void set(Y*, shared_ptr_control*) noexcept;
	};
	
	template <class T, class Y = T>
	constexpr static bool has_shared_from_this = detect_shared_from_this<T, Y>::value;
	
	
	template <class T, class Y, bool B>
	struct use_SFT_helper;
	
	
	template <class T, class Y>
	struct use_SFT_helper<T, Y, false>
	{
		constexpr static shared_ptr_control* use(Y*) noexcept
		{
			return nullptr;
		}
		
		constexpr static void set(Y*, shared_ptr_control*) noexcept
		{
			return;
		}
	};
	
	template <class T, class Y>
	struct use_SFT_helper<T, Y, true>
	{
		typedef typename Utils::remove_pointer<typename detect_shared_from_this<T, Y>::type>::type type;
		static shared_ptr_control* use(Y* ptr) noexcept
		{
			return ((enable_shared_from_this<type>*)ptr)->refer.ctrl;
		}
		
		static void set(Y* ptr, shared_ptr_control* ctrl) noexcept
		{
			assert(ptr);
			ptr->weak_assign(ctrl, ptr);
			/*assert(ctrl);
			++ctrl->refcount;
			ptr->ctrl = ctrl;
			ptr->ptr = ptr;*/
		}
	};
	
	
	template <class T, class Y = T>
	using use_SFT = use_SFT_helper<T, Y, has_shared_from_this<T, Y>>;
}


#endif