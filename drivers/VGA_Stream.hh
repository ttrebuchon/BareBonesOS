#ifndef INCLUDED_VGA_STREAM_HH
#define INCLUDED_VGA_STREAM_HH

#include <kernel/utils/streambuf.hh>
#include <kernel/utils/streambuf.hpp>
#include <drivers/VGA.hh>

namespace Drivers
{
	
	class VGAStreamBuf : public Utils::streambuf
	{
		private:
		char_type buffer[VGA::Width*VGA::Height];
		
		
		protected:
		
		
		virtual int sync();
		
		public:
		constexpr static size_t Width = VGA::Width;
		constexpr static size_t Height = VGA::Height;
		
		VGAStreamBuf();
	};
	
}

#endif