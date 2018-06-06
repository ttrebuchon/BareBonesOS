#include "Error.h"
#include <drivers/VGA.hh>
#include <Utils/int_to_str.h>
#include "Utility.hh"


void __do_kernel_panic(const char* msg, const char* file, const int line, const char* function)
{
    __DO_CLI__
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
    #ifdef __ENV_AARCH64__
    asm volatile ("hlt 0");
    #elif defined(__ENV_x86__)
    asm volatile ("hlt");
    #elif defined(__VS_CODE__)

    #else
    #error Unknown "hlt" asm for architecture
    #endif
    while (1) ;
}