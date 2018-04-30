#include "VGA.hh"
#include <Utils/string>
#include <Utils/int_to_str.h>
#include <kernel/Memory.h>

namespace Drivers
{
    bool VGA::initialized = false;
    size_t VGA::terminal_row = 0;
    size_t VGA::terminal_column = 0;
    uint8_t VGA::terminal_color = VGA::AttributeValue(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    #ifndef TESTING
    uint16_t* VGA::terminal_buffer = (uint16_t*) 0xB8000;
    #else
    uint16_t* vga_buf = new uint16_t[VGA::Width*VGA::Height];
    uint16_t* VGA::terminal_buffer = vga_buf;
    #endif





    void VGA::Init()
    {
        if (initialized)
        {
            return;
        }
        initialized = true;
        for (size_t y = 0; y < VGA::Height; ++y)
        {
            for (size_t x = 0; x < VGA::Width; ++x)
            {
                const size_t index = y*VGA::Width + x;
                terminal_buffer[index] = VGA::CellValue(' ', terminal_color);
            }
        }
    }

    void VGA::SetColor(const uint8_t color)
    {
        terminal_color = color;
    }

    void VGA::PutAt(const char c, const uint8_t color, const size_t x, const size_t y)
    {
        const size_t index = y*Width + x;
        terminal_buffer[index] = CellValue(c, color);
    }

    void VGA::PutChar(const char c)
    {
        if (c != '\n')
        {
            PutAt(c, terminal_color, terminal_column, terminal_row);   
        }
        else
        {
            terminal_column = Width-1;
        }
        if (++terminal_column == Width)
        {
            terminal_column = 0;
            if (++terminal_row == Height)
            {
                memcpy(terminal_buffer, &terminal_buffer[Width], 2*Width*(Height-1));
                terminal_row -= 1;
                kmemset(&terminal_buffer[(Height-1)*Width], 0, 2*Width);
            }
        }
    }

    void VGA::Write(const char* cstr, const size_t len)
    {
        for (size_t i = 0; i < len; ++i)
        {
            PutChar(cstr[i]);
        }
    }

    void VGA::Write(const char* cstr)
    {
        for (size_t i = 0; cstr[i] != 0; ++i)
        {
            PutChar(cstr[i]);
        }
    }

    void VGA::Write(const unsigned char* cstr)
    {
        for (size_t i = 0; cstr[i] != 0; ++i)
        {
            PutChar(cstr[i]);
        }
    }

    void VGA::Write(const Utils::string& str)
    {
        Write(str.c_str());
    }

    void VGA::Write(const void* addr)
    {
        int32_t tmp;
        addr_t n = (addr_t)addr;

        Write("0x");
        bool nZeroes = 1;


        for (int i = 28; i > 0; i -= 4)
        {
            tmp = (n >> i) & 0xF;
            if (tmp == 0 && nZeroes)
            {
                continue;
            }

            if (tmp >= 0xA)
            {
                nZeroes = false;
                PutChar(tmp-0xA+'a');
            }
            else
            {
                nZeroes = false;
                PutChar(tmp+'0');
            }
        }

        tmp = n & 0xF;
        if (tmp >= 0xA)
        {
            PutChar(tmp-0xA+'a');
        }
        else
        {
            PutChar(tmp+'0');
        }
    }

    void VGA::Write(const bool val)
    {
        if (val)
        {
            Write("true");
        }
        else
        {
            Write("false");
        }
    }


    void VGA::Write(const unsigned char n)
    {
        Write((unsigned short)n);
    }
    
    void VGA::Write(const unsigned short n)
    {
        char buf[256];
        if (c_uint_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }

    void VGA::Write(const unsigned int n)
    {
        char buf[256];
        if (c_uint_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }

    void VGA::Write(const unsigned long n)
    {
        char buf[256];
        if (c_uint_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }

    void VGA::Write(const unsigned long long n)
    {
        char buf[256];
        if (c_uint_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }






    void VGA::Write(const short n)
    {
        char buf[256];
        if (c_int_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }

    void VGA::Write(const int n)
    {
        char buf[256];
        if (c_int_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }

    void VGA::Write(const long n)
    {
        char buf[256];
        if (c_int_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }

    void VGA::Write(const long long n)
    {
        char buf[256];
        if (c_int_to_str(n, buf, 256))
        {
            Write(buf);
        }
    }


    void VGA::Write_HardcodedMark()
    {
        PutChar('M');
        PutChar('a');
        PutChar('r');
        PutChar('k');
        PutChar('\n');
    }
}


EXTERN_C void c_vga_write(const char* cstr)
{
    Drivers::VGA::Write(cstr);
}

EXTERN_C void c_vga_write_addr(const void* addr)
{
    Drivers::VGA::Write(addr);
}