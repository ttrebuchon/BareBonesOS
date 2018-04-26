#include "Error.h"
#include <drivers/VGA.hh>
#include <Utils/int_to_str.h>


void __do_kernel_panic(const char* msg, const char* file, const int line, const char* function)
{
    asm volatile ("cli");
    c_vga_write(file);
    c_vga_write("::");
    char line_str[256];

    if (c_int_to_str(line, line_str, 256))
    {
        c_vga_write(line_str);
        c_vga_write("::");
    }

    c_vga_write(function);

    c_vga_write("::");

    c_vga_write(msg);

    asm volatile("hlt");
    while (1) ;
}