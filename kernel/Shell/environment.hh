#ifndef INCLUDED_KERNEL_SHELL_ENVIRONMENT_HH
#define INCLUDED_KERNEL_SHELL_ENVIRONMENT_HH

#include <Common.h>
#include <kernel/Memory/Allocators/virtual.hh>
#include "result.hh"
#include <Utils/string>
#include <Utils/map>


namespace Kernel
{
	template <class Alloc>
	struct basic_environment_variables
	{
		private:
		template <class T>
		using alloc_type_rebind = typename Alloc::template rebind<T>::other;
		
		public:
		typedef Alloc allocator_type;
		typedef Utils::string string_type;
		
		
		
		Utils::map<string_type, string_type, Utils::less<string_type>, alloc_type_rebind<Utils::pair<const string_type, string_type>>> vars;
		
		basic_environment_variables(const allocator_type& a) : vars(alloc_type_rebind<Utils::pair<const string_type, string_type>>(a))
		{
			
		}
		
		basic_environment_variables() : basic_environment_variables(allocator_type())
		{
			
		}
		
		
	};
	
	
	typedef basic_environment_variables<Utils::allocator<char>> env_variables;
}

#endif