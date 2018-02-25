#include <drivers/VGA.hh>
#include <kernel/gdt.h>
#include <kernel/TSS.h>
#include <kernel/Interrupts.h>
#include <kernel/Interrupts/IDT.h>
#include <kernel/Registers.h>
#include <kernel/Memory/Paging.hh>
#include <kernel/Timer.h>
#include <kernel/utils/OrderedList.hh>
#include <kernel/KernelAllocator.hh>
#include <kernel/Memory/kheap.hh>
#include <kernel/Debug.h>
#include <kernel/Filesystem/initrd.hh>
#include <kernel/Filesystem/Node.hh>
#include <kernel/Filesystem/initrd/DirectoryNode.hh>
#include <drivers/Disk/AHCI.hh>
#include <drivers/PCI.hh>
#include <kernel/Task.hh>
#include <boot/multiboot.h>
#include <drivers/IDE/IDE.hh>
#define private public
#include <kernel/Memory/GDT.hh>


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

Kernel::gdt_entry gdt_table[4];
Kernel::Memory::GDTEntry gdt_table2[5];

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

    
    Kernel::Memory::init_gdt();
    
    
    // gdt_table[0] = Kernel::gdt_entry::nullEntry();
    
    // gdt_table[1] = Kernel::gdt_entry::nullEntry();
    // gdt_table[1].setLimit(0xfffffff);
    // gdt_table[1].setBase(0);
    // gdt_table[1].setType(0x9A);
    
    // gdt_table[2] = Kernel::gdt_entry::nullEntry();
    // gdt_table[2].setLimit(0xfffffff);
    // gdt_table[2].setBase(0);
    // gdt_table[2].setType(0x92);

    // gdt_table[3] = Kernel::gdt_entry::nullEntry();
    // gdt_table[3].setLimit(sizeof(Kernel::TSS));
    // gdt_table[3].setBase(reinterpret_cast<uint32_t>(Kernel::TSS::myTSS));
    // gdt_table[3].setType(0x89);

    // Kernel::gdt_entry::loadTable(gdt_table, 4);
    // Kernel::gdt_entry::reloadSegments();
    Drivers::VGA::Init();
    Drivers::VGA::Write("GDT Written\n");
    Kernel::Interrupts::sti();
    Drivers::VGA::Write("Initializing IDT...\n");
    Kernel::Interrupts::init_idt();
    Drivers::VGA::Write("IDT Initialized.\n");
    Drivers::VGA::Write("Initializing timer...\n");
    init_timer(50);
    Drivers::VGA::Write("Timer initialized.\n");
    ASSERT(init_esp != 0);
    ASSERT(mboot_ptr != 0);
    Drivers::VGA::Write("Initializing paging...\n");
    Kernel::Memory::init_paging();
    Drivers::VGA::Write("Paging initialized.\n");
    //Kernel::init_tasking();
    Drivers::VGA::Write("Tasking initialized.\n");

    Drivers::VGA::Write("Hello, kernel world!\nThis is a test.\n");

    Kernel::Memory::GDTEntry gdt2;
    gdt2.limit(65500);
    Kernel::gdt_entry gdt;
    gdt.setLimit(65500);
    auto lim = gdt.getLimit();
    auto lim2 = gdt2.limit();
    ASSERT(lim2 == 65500);
    if (lim != 65500)
    {
        Drivers::VGA::Write("Limit was encoded/decoded incorrectly!\n");
    }
    else
    {
        Drivers::VGA::Write("Limit was encoded/decoded correctly!\n");
    }

    gdt2.base(33554432);
    gdt.setBase(33554432);
    auto b = gdt.getBase();
    auto b2 = gdt2.base();
    ASSERT(b2 == 33554432);
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
        ASSERT(Kernel::Memory::kheap != 0x0);
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

    // for (int i = 0; i < 256; ++i)
    // {
    //     auto x = Drivers::PCI::ConfigReadWord(i, i, 0, 2);
    //     if (x == 0xFFFF)
    //     {
    //         Drivers::VGA::Write("x was 0xFFFF!\n");
    //     }
    // }
    

    if (Kernel::fork() != 0)
    {
        //Drivers::VGA::Write("First process running!\n");
        //while(1);
    }
    else
    {
        Drivers::VGA::Write("Second process running!\n");
    }

    Drivers::VGA::Write("Tasks length: ");
    Drivers::VGA::Write(Kernel::taskLength());
    Drivers::VGA::Write("\n");

    Drivers::VGA::Write("Testing sleeping (2s)...\n");
    sleep(2000);
    Drivers::VGA::Write("Done sleeping.\n");


    Drivers::VGA::Write("Initializing ATA Devices...\n");
    Drivers::IDE::Device::Initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
    ASSERT(Drivers::IDE::Device::Initialized());
    Drivers::VGA::Write("ATA Initialized.\n");
    Drivers::VGA::Write("Device 0 Present? ");
    Drivers::VGA::Write(Drivers::IDE::Device::Devices[0].reserved != 0);
    Drivers::VGA::Write("\n");
    if (Drivers::IDE::Device::Devices[0].reserved != 0)
    {
        Drivers::VGA::Write("Drive 0 Size: ");
        Drivers::VGA::Write(Drivers::IDE::Device::Devices[0].size);
        Drivers::VGA::Write("\n");
        Drivers::VGA::Write("Model: '");
        Drivers::VGA::Write(Drivers::IDE::Device::Devices[0].model);
        Drivers::VGA::Write("'\n");
    }

    for (int i = 1; i < 4; ++i)
    {
        if (Drivers::IDE::Device::Devices[i].reserved != 0)
        {
            Drivers::VGA::Write("Drive Size: ");
            Drivers::VGA::Write(Drivers::IDE::Device::Devices[i].size);
            Drivers::VGA::Write("\n");
            Drivers::VGA::Write("Model: '");
            Drivers::VGA::Write(Drivers::IDE::Device::Devices[i].model);
            Drivers::VGA::Write("'\n");
        }
    }
    

    Drivers::VGA::Write("Kernel main() is finished!!\n");
    return 0;
}