#include "DebugStream.hh"
#include <drivers/VGA.hh>
#include <drivers/COM.h>


namespace Kernel
{
	debug_streambuf::debug_streambuf() : Utils::streambuf(), buffer()
	{
		COM_init(COM1_PORT);
		setp(buffer, buffer + buffer_size);
	}

	int debug_streambuf::sync()
	{
		for (auto ptr = pbase(); ptr != pptr(); ++ptr)
		{
			COM_write(COM1_PORT, *ptr);
		}
		Drivers::VGA::Write(pbase(), pptr() - pbase());
		setp(buffer, buffer + buffer_size);
		return 0;
	}

	typename Utils::streambuf::int_type debug_streambuf::overflow(typename Utils::streambuf::int_type c)
	{
		this->sync();
		if (pptr() == pbase())
		{
			return this->sputc(c);
		}
		else
		{
			return traits_type::eof();
		}
	}
}