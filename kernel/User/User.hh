#ifndef INCLUDED_USER_USER_HH
#define INCLUDED_USER_USER_HH

#include <Common.h>
#include "User.h"
#include <kernel/Security/group.h>

namespace Kernel
{
	
	class User
	{
		private:
		user_t _user;
		
		public:
		
		
		constexpr const user_t& user() const noexcept
		{
			return _user;
		}
		
		constexpr uid_t id() const noexcept
		{
			return user().id;
		}
		
		constexpr const char* name() const noexcept
		{
			return user().name;
		}
	};
	
	
}

#endif