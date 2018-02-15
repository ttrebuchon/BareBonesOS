#include <drivers/VGA.hh>
#include <kernel/gdt.h>
#include <kernel/TSS.h>
#include <kernel/Interrupts.h>
#include <kernel/Interrupts/IDT.h>
#include <kernel/Registers.h>
#include <kernel/Memory/Paging.h>
#include <kernel/Timer.h>
#include <kernel/utils/OrderedList.hh>
#include <kernel/KernelAllocator.hh>
#include <kernel/Memory/kheap.h>
#include <kernel/Debug.h>
#include <kernel/Filesystem/initrd.hh>
#include <kernel/Filesystem/Node.hh>
#include <kernel/Filesystem/initrd/DirectoryNode.hh>
#include <drivers/Disk/AHCI.hh>
#include <drivers/PCI.hh>


#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross compiler .....dumbass."
#endif


#if !defined(__i386__)
#error "Wrong compiler...."
#endif

// static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
// {
//     return fg | bg << 4;
// }

// static inline uint16_t vga_entry(unsigned char c, uint8_t color)
// {
//     return (uint16_t) c | (uint16_t) color << 8;
// }

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
    {
        ++len;
    }
    return len;
}

void handler04(Registers_t regs)
{
    Drivers::VGA::Write("handler04()\n");
}

static_assert(sizeof(Registers_t) == 64);


//In Task_c.c
extern "C" uint32_t init_esp;

#if defined(__cplusplus)
extern "C"
#endif
int main(struct multiboot* mboot_ptr, uint32_t initial_stack)
{
	init_esp = initial_stack;
    Kernel::TSS _myTSS{0};
    Kernel::TSS::myTSS = &_myTSS;

    Kernel::Interrupts::cli();

    Drivers::VGA::Init();
    Drivers::VGA::Write("Writing GDT...\n");

    Kernel::gdt_entry gdt_table[4];
    
    gdt_table[0] = Kernel::gdt_entry::nullEntry();
    
    gdt_table[1] = Kernel::gdt_entry::nullEntry();
    gdt_table[1].setLimit(0xfffffff);
    gdt_table[1].setBase(0);
    gdt_table[1].setType(0x9A);
    
    gdt_table[2] = Kernel::gdt_entry::nullEntry();
    gdt_table[2].setLimit(0xfffffff);
    gdt_table[2].setBase(0);
    gdt_table[2].setType(0x92);

    gdt_table[3] = Kernel::gdt_entry::nullEntry();
    gdt_table[3].setLimit(sizeof(Kernel::TSS));
    gdt_table[3].setBase(reinterpret_cast<uint32_t>(Kernel::TSS::myTSS));
    gdt_table[3].setType(0x89);

    Kernel::gdt_entry::loadTable(gdt_table, 4);
    Kernel::gdt_entry::reloadSegments();
    Drivers::VGA::Init();
    Drivers::VGA::Write("GDT Written\n");
    Kernel::Interrupts::sti();
    Drivers::VGA::Write("Initializing IDT...\n");
    Kernel::Interrupts::init_idt();
    Drivers::VGA::Write("IDT Initialized.\n");
    Drivers::VGA::Write("Initializing timer...\n");
    init_timer(50);
    Drivers::VGA::Write("Timer initialized.\n");
    Drivers::VGA::Write("Initializing paging...\n");
    init_paging();
    Drivers::VGA::Write("Paging initialized.\n");

    Drivers::VGA::Write("Hello, kernel world!\nThis is a test.\n");


    Kernel::gdt_entry gdt;
    gdt.setLimit(65500);
    auto lim = gdt.getLimit();
    if (lim != 65500)
    {
        Drivers::VGA::Write("Limit was encoded/decoded incorrectly!\n");
    }
    else
    {
        Drivers::VGA::Write("Limit was encoded/decoded correctly!\n");
    }

    gdt.setBase(33554432);
    auto b = gdt.getBase();
    if (b != 33554432)
    {
        Drivers::VGA::Write("Base was encoded/decoded incorrectly!\n");
    }
    else
    {
        Drivers::VGA::Write("Base was encoded/decoded correctly!\n");
    }


    

    Kernel::Interrupts::register_interrupt_handler(0x4, &handler04);

    asm volatile ("int $0x4");

    

    
    {
        ASSERT(kheap != 0x0);
        auto y = new int;
        auto z = new int;
        delete y;
        delete z;
        
        auto x = new int[40];
        delete[] x;
    }

    auto someMem = kmalloc(400, 0, 0x0);
    *(int*)someMem = 1;
    auto spanPtr = (unsigned long*)((int*)someMem + 1);
    *spanPtr = sizeof(int) + sizeof(unsigned long) + 3;
    auto namePtr = (char*)(spanPtr + 1);
    namePtr[0] = 'F';
    namePtr[1] = 'S';
    namePtr[2] = 0;

    auto fs = Kernel::Filesystem::init_initrd(someMem);
    Drivers::VGA::Write("Root Node Name: ");
    Drivers::VGA::Write(fs->name);
    Drivers::VGA::Write("\n");

    for (int i = 0; i < 256; ++i)
    {
        auto x = Drivers::PCI::ConfigReadWord(i, i, 0, 2);
        if (x == 0xFFFF)
        {
            Drivers::VGA::Write("x was 0xFFFF!\n");
        }
    }
    

    Drivers::VGA::Write("Kernel main() is finished!!\n");
    return 0;
}