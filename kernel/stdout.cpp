#include <Utils/iostream>
#include <Drivers/VGA_Stream.hh>
#include "DebugStream.hh"

//static Drivers::VGAStreamBuf buf;

//typedef Drivers::VGAStreamBuf cout_streambuf_type;
typedef Kernel::debug_streambuf cout_streambuf_type;

unsigned char buf_raw[sizeof(cout_streambuf_type)];
unsigned char cout_raw[sizeof(Utils::ostream)];

static Utils::streambuf* get_stdout_buf()
{
    return reinterpret_cast<cout_streambuf_type*>(buf_raw);
}

void initialize_stdout()
{
    new (buf_raw) cout_streambuf_type();
    new (cout_raw) Utils::ostream(get_stdout_buf());

    // Utils::cout.rdbuf(new cout_streambuf_type());
}


namespace Utils
{
    
    ostream& cout = *reinterpret_cast<ostream*>(cout_raw);
}