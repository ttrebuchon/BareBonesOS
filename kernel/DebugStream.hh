#ifndef INCLUDED_KERNEL_DEBUGSTREAM_HH
#define INCLUDED_KERNEL_DEBUGSTREAM_HH

#include <Common.h>
#include <Utils/streambuf>

namespace Kernel
{
	class debug_streambuf : public Utils::streambuf
	{
		protected:
		constexpr static size_t buffer_size = 4000;
		char_type buffer[buffer_size];

		virtual int sync() override;
		virtual typename Utils::streambuf::int_type overflow(typename Utils::streambuf::int_type c = traits_type::eof()) override;

		public:

		debug_streambuf();

	};
}

#endif