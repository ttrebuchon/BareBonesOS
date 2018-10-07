#ifndef INCLUDED_KERNEL_SHELL_COMMAND_HH
#define INCLUDED_KERNEL_SHELL_COMMAND_HH

#include <Common.h>
#include <Utils/shared_ptr>
#include <Utils/string>
#include <Utils/vector>
#include <kernel/Memory/Allocators/virtual.hh>

namespace Kernel::detail::shell
{
	
	struct CommandContinuation;
	
	class Command
	{
		public:
		typedef Memory::virtual_allocator<uint8_t> allocator_type;
		//typedef Utils::basic_string<char, Utils::char_traits<char>, typename allocator_type::template rebind<char>::other> string_type;
		typedef Utils::string string_type;
		
		protected:
		
		public:
		string_type command;
		Utils::vector<string_type, typename allocator_type::template rebind<string_type>::other> args;
		Utils::shared_ptr<CommandContinuation> continuation;
		
		Command(const allocator_type& a)
				:
				command(),
				args(typename allocator_type::template rebind<string_type>::other(a)),
				continuation(nullptr)
		{
			
		}
		
		
		
		
	};
	
	struct CommandContinuation
	{
		typedef typename Command::string_type string_type;
		
		string_type symbol;
		Command command;
		
		CommandContinuation(const string_type& symbol, const Command& cmd) : symbol(symbol), command(cmd)
		{
			
		}
	};
}

#endif