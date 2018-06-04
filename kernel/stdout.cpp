#include <Utils/iostream>
#include <Drivers/VGA_Stream.hh>

//static Drivers::VGAStreamBuf buf;



unsigned char buf_raw[sizeof(Drivers::VGAStreamBuf)];
unsigned char cout_raw[sizeof(Utils::ostream)];

static Utils::streambuf* get_stdout_buf()
{
    return reinterpret_cast<Drivers::VGAStreamBuf*>(buf_raw);
}

void initialize_stdout()
{

    new (buf_raw) Drivers::VGAStreamBuf();
    new (cout_raw) Utils::ostream(get_stdout_buf());

    // Utils::cout.rdbuf(new Drivers::VGAStreamBuf());
}


namespace Utils
{
    
    ostream& cout = *reinterpret_cast<ostream*>(cout_raw);
}