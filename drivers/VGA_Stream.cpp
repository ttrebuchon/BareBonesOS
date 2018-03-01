#include "VGA_Stream.hh"


namespace Drivers
{
	
	
	VGAStreamBuf::VGAStreamBuf() : Utils::streambuf(), buffer()
	{
		setp(buffer, buffer + Width*Height);
		
	}
	
	int VGAStreamBuf::sync()
	{
		VGA::Write(buffer);
		setp(buffer, buffer + Width*Height);
		return 0;
	}
	
}