#ifndef INCLUDED_MEMORY_ALLOCATORS_CHOICE_HPP
#define INCLUDED_MEMORY_ALLOCATORS_CHOICE_HPP

#include "choice_allocator.hh"
#include <Utils/mutex>

namespace Kernel { namespace Memory
{
	namespace detail
	{
		template <class Alloc>
		choice_option_set<Alloc>::choice_option_set(const Alloc& a) : alloc(a), options(a), mut()
		{
			
		}
		
		
		
		
		template <class Alloc>
		template <class T>
		void choice_option_set<Alloc>::add_alloc(const T& a)
		{
			typedef typename T::value_type obj_type;
			typedef typename allocator_type::template rebind<choice_alloc_option<T, sizeof(obj_type), alignof(obj_type)>>::other option_alloc_t;
			option_alloc_t opt_alloc(alloc);
			auto obj = opt_alloc.allocate(1);
			opt_alloc.construct(obj, a);
			auto ptr = Utils::shared_ptr<choice_option>(obj, alloc);
			alloc_spec_t key(sizeof(obj_type), alignof(obj_type));
			
			{
				Utils::lock_guard<mutex_type> lock(mut);
				options[key] = ptr;
			}
		}
		
		
		template <class Alloc>
		Utils::shared_ptr<choice_option> choice_option_set<Alloc>::get(const alloc_spec_t& s)
		{
			Utils::lock_guard<s_mutex_type> lock(s_mut);
			if (options.count(s))
			{
				return options.at(s);
			}
			return nullptr;
		}
		
		
		template <class Alloc>
		Utils::shared_ptr<choice_option_set<Alloc>> choice_option_set<Alloc>::Make(const Alloc& a)
		{
			typedef typename Alloc::template rebind<choice_option_set>::other set_alloc_t;
			set_alloc_t set_alloc(a);
			auto set = set_alloc.allocate(1);
			new (set) choice_option_set(a);
			return Utils::shared_ptr<choice_option_set>(set,
			[](auto x) -> void
			{
				delete x;
			}, a);
		}
	}
	
	
	
	
	
	
	
	
	
	template <class T, class A>
	choice_allocator<T, A>::choice_allocator(const A& a) : set(nullptr)
	{
		set = detail::choice_option_set<A>::Make(a);
	}
}
}

#endif