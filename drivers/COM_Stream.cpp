#include "COM_Stream.hh"


namespace Drivers
{
	COMStreamBuf::COMStreamBuf(uint16_t port) : Utils::streambuf(), buffer(), port(port)
	{
		setp(buffer, buffer + buffer_size);
		COM_init(port);
	}
	
	int COMStreamBuf::sync()
	{
		for (auto ptr = pbase(); ptr != pptr(); ++ptr)
		{
			COM_write(port, *ptr);
		}
		setp(buffer, buffer + buffer_size);
		return 0;
	}
}