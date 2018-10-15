#ifndef INCLUDED_KERNEL_SHELL_PARSING_HH
#define INCLUDED_KERNEL_SHELL_PARSING_HH

#include <Utils/string>
#include <Utils/vector>

namespace Kernel::Memory
{
	template <class>
	class virtual_allocator;
}

namespace Kernel
{
	template <class>
	struct basic_environment_variables;
}

namespace Kernel::detail::shell
{
	class Command;
	
	bool shell_input_parse(const char* start, const char* end, const Kernel::Memory::virtual_allocator<uint8_t>&,basic_environment_variables<Memory::virtual_allocator<uint8_t>>*, Command*);
	
	//typedef Utils::basic_string<char, Utils::Char_Traits<char>, Kernel::Memory::virtual_allocator<char>> utils_string_type;
	typedef Utils::string utils_string_type;
	
	bool path_string_parse(const char* start, const char* end, const Kernel::Memory::virtual_allocator<uint8_t>&, Utils::vector<utils_string_type, typename Kernel::Memory::virtual_allocator<utils_string_type>>&);
	
}

#endif