#include "VGA.hh"
//#include <kernel/utils/String.hh>

namespace Drivers
{
    bool VGA::initialized = false;
    size_t VGA::terminal_row = 0;
    size_t VGA::terminal_column = 0;
    uint8_t VGA::terminal_color = VGA::AttributeValue(VGA::Color::LIGHT_GREY, VGA::Color::BLACK);
    uint16_t* VGA::terminal_buffer = (uint16_t*) 0xB8000;





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
                terminal_row = 0;
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

    // void VGA::Write(const Utils::String str)
    // {
    //     //TODO
    // }

    void VGA::Write(const void* addr)
    {
        int32_t tmp;
        uint32_t n = (uint32_t)addr;

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

}


EXTERN_C void c_vga_write(const char* cstr)
{
    Drivers::VGA::Write(cstr);
}

EXTERN_C void c_vga_write_addr(const void* addr)
{
    Drivers::VGA::Write(addr);
}