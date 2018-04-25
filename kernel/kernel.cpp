#define private public
#define protected public
#include <drivers/VGA.hh>
#include <kernel/TSS.h>
#include <kernel/Interrupts.h>
#include <kernel/Interrupts/IDT.h>
#include <kernel/Registers.h>
#include <kernel/Memory/Paging.hh>
#include <kernel/Timer.h>
#include <Utils/OrderedList.hh>
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
#include <kernel/Memory/GDT.hh>
#include <drivers/IDE/DMA.hh>
#include <kernel/Filesystem/FileNode.hh>
#include <kernel/Filesystem/DirectoryNode.hh>

#include <drivers/VGA_Stream.hh>
#include <Utils/iostream>
#include <boot/MultiBoot.hh>


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

Kernel::Memory::GDTEntry gdt_table2[5];

//In Task_c.c
extern "C" uint32_t init_esp;


void testPaging();

#if defined(__cplusplus)
extern "C"
#endif
int main(struct multiboot* mboot_ptr, uint32_t initial_stack)
{
	init_esp = initial_stack;
    // TaskStateSegment_x86_t myTSS;
    // Kernel::TSS _myTSS{0};
    // Kernel::TSS::myTSS = &_myTSS;

    Kernel::Interrupts::cli();

    Drivers::VGA::Init();
    
    
    boot::mboot = new boot::MultiBoot(mboot_ptr);
    
    Drivers::VGA::Write("Writing GDT...\n");

    
    Kernel::Memory::init_gdt();
    
    Drivers::VGA::Init();
    Drivers::VGA::Write("GDT Written\n");
    Kernel::Interrupts::sti();
    Drivers::VGA::Write("Initializing IDT...\n");
    Kernel::Interrupts::init_idt();
    Drivers::VGA::Write("IDT Initialized.\n");
    Drivers::VGA::Write("Initializing timer...\n");
    init_timer(500);
    Drivers::VGA::Write("Timer initialized.\n");
    ASSERT(init_esp != 0);
    ASSERT(mboot_ptr != 0);
    Drivers::VGA::Write("Initializing paging...\n");
    Kernel::Memory::init_paging();
    Drivers::VGA::Write("Paging initialized.\n");
    // Drivers::VGA::Write("Testing paging...\n");
    // testPaging();
    // Drivers::VGA::Write("Paging tested.\n");
    // ASSERT(false);
    Kernel::init_tasking();
    Drivers::VGA::Write("Tasking initialized.\n");

    Drivers::VGA::Write("Hello, kernel world!\nThis is a test.\n");

    Kernel::Memory::GDTEntry gdt;
    gdt.limit(65500);
    auto lim = gdt.limit();
    ASSERT(lim == 65500);

    gdt.base(33554432);
    auto b = gdt.base();
    ASSERT(b == 33554432);


    Drivers::VGAStreamBuf vgaBuf;
    Utils::ostream out(&vgaBuf);

    Kernel::Interrupts::register_interrupt_handler(0x4, &handler04);

    asm volatile ("int $0x4");


    out << "GDT Table Address: " << (void*)&Kernel::Memory::gdt_table << "\n";
    out << "Initial ESP: " << (void*)init_esp << "\n";
    addr_t ebp;
    asm volatile("mov %%esp, %0" : "=r"(ebp));
    out << "EBP: " << (void*)ebp << "\n";
    out.flush();




    // try
    // {
    //     throw 4;
    // }
    // catch (...)
    // {
    //     assert(false);
    //     out << "CAUGHT!\n";
    //     out.flush();
    // }
    // assert(false);
    
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

    Drivers::PCI::Initialize();

    for (int i = 0; i < 256; ++i)
    {
        for (int n = 0; n < 256; ++n)
        {
            auto x = Drivers::PCI::ConfigReadWord(i, n, 0, 0);
            if (x != 0xFFFF)
            {
                Drivers::VGA::Write("x was not 0xFFFF! At x: ");
                Drivers::VGA::Write((void*)x);
                Drivers::VGA::Write("\n");
            }

            auto y = Drivers::PCI::ConfigReadWord(i, n, 2, 0);
            if (y != 0xFFFF)
            {
                Drivers::VGA::Write("y was not 0xFFFF! At y: ");
                Drivers::VGA::Write((void*)y);
                Drivers::VGA::Write("\n");
            }
        }
    }
    Drivers::PCIDevice_t dev;
        
    for (int i = 0; i < 256; ++i)
    {
        dev.data.busNo = i;
        for (int j = 0; j < 31; ++j)
        {
            dev.data.deviceNo = j;
            dev.data.fieldNo = (((uint16_t)Drivers::PCIRegister::BAR4) & 0xFC) >> 2;
            ASSERT(dev.data.fieldNo != 0);
            for (int k = 0; k < 8; ++k)
            {
                dev.data.functionNo = k;
                dev.data.zero = 0;
                dev.data.reserved = 0;
                dev.data.enabled = 1;
                uint32_t expected = (((uint32_t)dev.data.busNo << 16) | ((uint32_t)dev.data.deviceNo << 11) | ((uint32_t)dev.data.functionNo << 8) | ((uint32_t)dev.data.fieldNo << 2) | 0x80000000);
                uint32_t actual = dev.data;
                if (actual != expected)
                {
                    Drivers::VGA::Write((void*)actual);
                    Drivers::VGA::Write("\n");
                    Drivers::VGA::Write((void*)expected);
                    Drivers::VGA::Write("\n");
                }
                ASSERT(actual == expected);
                if (dev.read(Drivers::PCIRegister::VendorID) != 0xFFFF)
                {
                    Drivers::VGA::Write(dev.data.busNo);
                    Drivers::VGA::Write(" : ");
                    Drivers::VGA::Write(dev.data.deviceNo);
                    Drivers::VGA::Write(" : ");
                    Drivers::VGA::Write(dev.data.functionNo);
                    Drivers::VGA::Write(" : ");
                    Drivers::VGA::Write((void*)(addr_t)dev.read(Drivers::PCIRegister::VendorID));
                    Drivers::VGA::Write(" : ");
                    Drivers::VGA::Write((void*)(addr_t)dev.read(Drivers::PCIRegister::DeviceID));
                    Drivers::VGA::Write(" : ");
                    Drivers::VGA::Write((void*)(addr_t)dev.type());
                    Drivers::VGA::Write(" : ");
                    Drivers::VGA::Write((void*)(addr_t)dev.read(Drivers::PCIRegister::ClassCode));
                    Drivers::VGA::Write(" : ");
                    Drivers::VGA::Write((void*)(addr_t)dev.read(Drivers::PCIRegister::Subclass));
                    Drivers::VGA::Write("      -      ");
                    Drivers::VGA::Write((void*)(addr_t)dev.data);
                    Drivers::VGA::Write("\n");
                    if (dev.data.busNo != dev.secondaryBus())
                    {
                        dev.data.busNo = dev.secondaryBus();
                        Drivers::VGA::Write(dev.data.busNo);
                        Drivers::VGA::Write(" : ");
                        Drivers::VGA::Write(dev.data.deviceNo);
                        Drivers::VGA::Write(" : ");
                        Drivers::VGA::Write(dev.data.functionNo);
                        Drivers::VGA::Write(" : ");
                        Drivers::VGA::Write((void*)(addr_t)dev.read(Drivers::PCIRegister::VendorID));
                        Drivers::VGA::Write(" : ");
                        Drivers::VGA::Write((void*)(addr_t)dev.read(Drivers::PCIRegister::DeviceID));
                        Drivers::VGA::Write(" : ");
                        Drivers::VGA::Write((void*)(addr_t)dev.type());
                        Drivers::VGA::Write("      -      ");
                        Drivers::VGA::Write((void*)(addr_t)dev.data);
                        Drivers::VGA::Write("\n");
                        Drivers::VGA::Write("\n");
                    }
                    

                    
                }
            }
        }
    }


    Drivers::VGA::Write("Testing sleeping (2s)...\n");
    sleep(2);
    Drivers::VGA::Write("Done sleeping.\n");
    

    if (Kernel::fork() != 0)
    {
        Drivers::VGA::Write("First process running!\n");
        assert(Kernel::taskLength() > 0);
        while(1);
    }
    else
    {
        //Drivers::VGA::Init();
        Drivers::VGA::Write("Second process running!\n");
        while (1);
    }

    Drivers::VGA::Write("Tasks length: ");
    Drivers::VGA::Write(Kernel::taskLength());
    Drivers::VGA::Write("\n");

    


    // Drivers::VGA::Write("Initializing ATA Devices...\n");
    // //Drivers::IDE::Device::Initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
    // Drivers::IDE::Device::Initialize();
    // ASSERT(Drivers::IDE::Device::Initialized());
    // Drivers::VGA::Write("ATA Initialized.\n");
    // Drivers::VGA::Write("Device 0 Present? ");
    // Drivers::VGA::Write(Drivers::IDE::Device::Devices[0].reserved != 0);
    // Drivers::VGA::Write("\n");
    
    // for (int i = 0; i < 4; ++i)
    // {
    //     if (Drivers::IDE::Device::Devices[i].reserved != 0)
    //     {
    //         Drivers::VGA::Write("Drive: ");
    //         Drivers::VGA::Write(i);
    //         Drivers::VGA::Write("\n");
    //         Drivers::VGA::Write("Drive Size: ");
    //         Drivers::VGA::Write(Drivers::IDE::Device::Devices[i].size);
    //         Drivers::VGA::Write("\n");
    //         Drivers::VGA::Write("Model: '");
    //         Drivers::VGA::Write(Drivers::IDE::Device::Devices[i].model);
    //         Drivers::VGA::Write("'\n");
    //         Drivers::VGA::Write("Signature: ");
    //         Drivers::VGA::Write((void*)(addr_t)Drivers::IDE::Device::Devices[i].signature);
    //         Drivers::VGA::Write("\n");

    //         // for (int j = 0; j < 40; ++j)
    //         // {
    //         //     if (Drivers::IDE::Device::Devices[i].model[j] != 0)
    //         //     {
    //         //         Drivers::VGA::Write((unsigned int)Drivers::IDE::Device::Devices[i].model[j]);
    //         //         Drivers::VGA::Write(", ");
    //         //     }
    //         //     else
    //         //     {
    //         //         break;
    //         //     }
    //         // }
    //         // Drivers::VGA::Write("\n");
    //     }
    // }


    out << "Testing! " << 4 << "\n";
    out.flush();
    Drivers::IDE::Device::Initialize();
    Drivers::IDE::Device pm(Drivers::IDE::Channel::Primary, Drivers::IDE::Role::Master);
    if (pm.reserved)
    {
        // out << "Model: ";
        //out.flush();
        //Drivers::VGA::Write(pm.model);
        out << "Model: " << pm.model << "\n";
        out.flush();
    }
    
    Drivers::IDE::Device ps(Drivers::IDE::Channel::Primary, Drivers::IDE::Role::Slave);
    if (ps.reserved)
    {
        //out << "\nModel: ";
        //out.flush();
        // Drivers::VGA::Write(ps.model);
        //out << "\n";
        //out.flush();
        out << "Model: " << ps.model << "\n";
        out.flush();
    }


    Drivers::IDE::Device sm(Drivers::IDE::Channel::Secondary, Drivers::IDE::Role::Master);
    if (sm.reserved)
    {
        //out << "Model: ";
        //out.flush();
        // Drivers::VGA::Write(sm.model);
        out << "Model: " << sm.model << "\n";
        out.flush();
    }
    
    
    Drivers::IDE::Device ss(Drivers::IDE::Channel::Secondary, Drivers::IDE::Role::Slave);
    if (ss.reserved)
    {
        //out << "\nModel: ";
        //out.flush();
        Drivers::VGA::Write(ss.model);
        //out << "\n";
        //out.flush();
        out << "Model: " << ss.model << "\n";
        out.flush();
    }

    out << "Testing DMADrive...\n";
    out.flush();
    Drivers::IDE::DMADrive dmdrive(Drivers::IDE::Channel::Primary, Drivers::IDE::Role::Master);

    auto sec1 = dmdrive.readSector(0, 512);
    out << "Returned.\n";
    out.flush();
    uint32_t sec1_1 = *(uint32_t*)sec1;
    out << (void*)sec1_1 << "\n";
    out << (void*)(uint32_t)sec1[511] << "\n";
    for (int i = 0; i < 512; ++i)
    {
        if (sec1[i] != 1)
        {
            out << (uint32_t)sec1[i] << "\n";
        }
    }

    free(sec1);


    out << "Kernel main() is finished!!\n";
    out.flush();
    //Drivers::VGA::Write("Kernel main() is finished!!\n");
    return 0;
}

void testPaging()
{
    asm volatile ("cli");
    Drivers::VGA::Write("KPlacement: ");
    Drivers::VGA::Write((void*)kPlacement);
    Drivers::VGA::Write("\n");
    // ASSERT(false);
    using namespace Kernel;
    using namespace Memory;

    const addr_t ADDR   = 0xF0000000;
    const addr_t ADDR2  = 0xF0200000;

    const size_t len = 0x1000;

    auto current_dir = PageDirectory::Current;

    current_dir->switch_to();
    TRACE("SWITCHED.\n");

    ASSERT(current_dir->map((ADDR - 0x0000), ADDR, len, true, false, false));
    unsigned char* a1 = (unsigned char*)ADDR;
    current_dir->at((void*)ADDR)->flush();
    // current_dir->flush((void*)ADDR);

    ASSERT(current_dir);
    //ASSERT(current_dir->map((ADDR2 - 0x0000), ADDR, len, true, false, false));
    unsigned char* a2 = (unsigned char*)ADDR2;

    // current_dir->flush((void*)ADDR2);
    current_dir->at((void*)ADDR2)->flush();


    // auto phys = virtual_to_physical(current_dir, (const void*)ADDR);
    auto phys = current_dir->physical((void*)ADDR);
    ASSERT(phys != 0);
    Drivers::VGA::Write(phys);
    Drivers::VGA::Write("\n");

    // for (volatile int i = 0; i < 100; ++i)
    // {
    //     ASSERT(current_dir->flush());
    // }
    
    //static_assert(alignof(PageDirectory::_Table) == 1024*4);
    TRACE(alignof(PageDirectory::_Table));
    TRACE("\n\n");
    
    auto p = current_dir->at((void*)ADDR);
    addr_t p_addr = (addr_t)(void*)p;
    addr_t p_phys = (addr_t)current_dir->physical(&**p);
    ASSERT(sizeof(**p) == sizeof(PageDirectory::_Page));
    
    Drivers::VGA::Write("P_Virtual: ");
    Drivers::VGA::Write((void*)&**p);
    Drivers::VGA::Write("\n");

    Drivers::VGA::Write("P_Physical: ");
    Drivers::VGA::Write((void*)p_phys);
    Drivers::VGA::Write("\n");
    addr_t dir_phys = (addr_t)current_dir->physical(current_dir);
    auto pTable = current_dir->table(p_addr/(1024*0x1000));
    ASSERT(pTable);
    void* pTable_phys = current_dir->physical(&**pTable);
    size_t tIndex = p_addr >> 22;
    addr_t tmp = (((addr_t)current_dir->thisPhysical()) + tIndex*4);
    ASSERT(&**pTable == *reinterpret_cast<void**>(&tmp));
    Drivers::VGA::Write("Table_Physical: ");
    Drivers::VGA::Write((void*)pTable_phys);
    Drivers::VGA::Write("\n");
    Drivers::VGA::Write("Dir_Physical: ");
    Drivers::VGA::Write((void*)dir_phys);
    Drivers::VGA::Write("\n");
    Drivers::VGA::Write("Dir_Virtual: ");
    Drivers::VGA::Write((void*)current_dir);
    Drivers::VGA::Write("\n");
    Drivers::VGA::Write("Stored Dir_Physical: ");
    Drivers::VGA::Write((void*)current_dir->thisPhysical());
    Drivers::VGA::Write("\n");
    ASSERT(p);
    Drivers::VGA::Write((void*)((addr_t)p->frame()));
    ASSERT((ADDR & 0xfffff000) == ADDR);
    Drivers::VGA::Write("\n");
    

    for (int i = 0; i < len; ++i)
    {
        a1[i] = (i % 256);
    }
    ASSERT((**current_dir->at((void*)ADDR)).accessed);

    // auto p = current_dir->getPage(ADDR, false);
    p = current_dir->at((void*)ADDR);
    ASSERT(p);
    Drivers::VGA::Write((void*)((addr_t)p->frame() << 12));
    ASSERT((ADDR & 0xfffff000) == ADDR);
    Drivers::VGA::Write("\n");
    
    current_dir->flush();

    for (int i = 0; i < len; ++i)
    {
        a1[i] = (i % 256);
    }
    ASSERT(current_dir->map((ADDR2 - 0x0000), ADDR, len, true, false, false));
    current_dir->at((void*)ADDR2)->flush();
    ASSERT(current_dir->physical(a1) == current_dir->physical(a2));
    for (int i = 0; i < len; ++i)
    {
        TRACE((uint32_t)i);
        TRACE("\n");
        ASSERT(a2[i] == (i % 256));
    }
    asm volatile ("sti");
}