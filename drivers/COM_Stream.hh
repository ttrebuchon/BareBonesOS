#ifndef INCLUDED_DRIVERS_COM_STREAM_HH
#define INCLUDED_DRIVERS_COM_STREAM_HH

#include <Common.h>
#include "COM.h"
#include <Utils/streambuf>

namespace Drivers
{
	class COMStreamBuf : public Utils::streambuf
	{
		private:
		constexpr static size_t buffer_size = 2000;
		char_type buffer[buffer_size];
		
		
		protected:
		uint16_t port;
		
		
		virtual int sync();
		
		public:
		
		COMStreamBuf(uint16_t port);
	};
}



#endif