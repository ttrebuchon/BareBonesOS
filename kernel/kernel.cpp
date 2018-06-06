#include <drivers/VGA.hh>
#include <Kernel/Initialize.hh>
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
#include <drivers/AHCI/AHCI.hh>
#include <drivers/PCI.hh>
#include <kernel/Task.hh>
#include <boot/multiboot.h>
#include <drivers/ATA/ATA.hh>
#include <kernel/Memory/GDT.hh>
#include <drivers/ATA/DMA.hh>
#include <kernel/Filesystem/FileNode.hh>
#include <kernel/Filesystem/DirectoryNode.hh>

#include <drivers/VGA_Stream.hh>
#include <Utils/iostream>
#include <boot/MultiBoot.hh>

#include <kernel/Scheduling/context.h>
#include <kernel/Scheduling/thread.h>

#include <Utils/iostream>

#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross compiler .....dumbass."
#endif


// #if !defined(__i386__)
// #error "Wrong compiler...."
// #endif

// static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
// {
//     return fg | bg << 4;
// }

// static inline uint16_t vga_entry(unsigned char c, uint8_t color)
// {
//     return (uint16_t) c | (uint16_t) color << 8;
// }

// size_t strlen(const char* str)
// {
//     size_t len = 0;
//     while (str[len])
//     {
//         ++len;
//     }
//     return len;
// }

void handler04(Registers_t regs)
{
    Drivers::VGA::Write("handler04()\n");
}

static_assert(sizeof(Registers_t) == 64);

Kernel::Memory::GDTEntry gdt_table2[5];

//In Task_c.c
extern "C" uint32_t init_esp;




void ktest_gdt();
void ktest_pci();
void ktest_threading();
void ktest_disks();


#if defined(__cplusplus)
extern "C"
#endif
int main(struct multiboot* mboot_ptr, uint32_t initial_stack)
{
    int testx;
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
    
    Drivers::VGA::Write("Initializing IDT...\n");
    Kernel::Interrupts::init_idt();
    Drivers::VGA::Write("IDT Initialized.\n");
    Drivers::VGA::Write("Initializing timer...\n");
    init_timer(500);
    //init_timer(5000);
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
    Kernel::init_kernel_threads();
    Drivers::VGA::Write("Threads initialized.\n");


    initialize_stdlib();

    // Ensure interrupts are enabled, they were left off during initialization
    while (Kernel::Interrupts::block_interrupt_counter() > 0)
    {
        Kernel::Interrupts::sti();
    }
    ASM_CODE("sti");








    Drivers::VGAStreamBuf* vgaBuf = new Drivers::VGAStreamBuf();
    Utils::ostream out(vgaBuf);

    Kernel::Interrupts::register_interrupt_handler(0x4, &handler04);

    ASM_CODE("int $0x4");

    std::cout << "GDT Table Address: " << (void*)&Kernel::Memory::gdt_table << "\n";
    std::cout << "Initial ESP: " << (void*)init_esp << "\n";
    addr_t ebp;
    ASM_CODE("mov %%esp, %0" : "=r"(ebp));
    std::cout << "EBP: " << (void*)ebp << std::endl;


    // try
    // {
    //     throw 4;
    // }
    // catch (...)
    // {
    //     assert(false);
    //     std::cout << "CAUGHT!\n";
    //     out.flush();
    // }
    // assert(false);
    
    // {
    //     ASSERT(Kernel::Memory::kheap != 0x0);
    //     auto y = new int;
    //     auto z = new int;
    //     delete y;
    //     delete z;
        
    //     auto x = new int[40];
    //     delete[] x;
    // }

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

    ktest_pci();
    ktest_threading();
    ktest_disks();



    std::cout << "Kernel main() is finished!!" << std::endl;
    return 0;
}


void ktest_gdt()
{
    Kernel::Memory::GDTEntry gdt;
    gdt.limit(65500);
    auto lim = gdt.limit();
    ASSERT(lim == 65500);

    gdt.base(33554432);
    auto b = gdt.base();
    ASSERT(b == 33554432);
}

void ktest_pci()
{
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
}

void ktest_threading()
{
    Drivers::VGA::Write("CLI Count: ");
    Drivers::VGA::Write(Kernel::Interrupts::block_interrupt_counter());
    Drivers::VGA::Write("\n");

    assert(Kernel::Interrupts::block_interrupt_counter() == 0);


    
    int thread2_marker = 0;
    pthread_t tid;
    Kernel::thread_create(&tid, [&thread2_marker]() -> void
    {
        {
            Kernel::Interrupts::irq_guard lock;
            Drivers::VGA::Write("Thread 2 executing!\n");
        }
        assert(Kernel::Interrupts::block_interrupt_counter() == 0);
        thread2_marker++;
    });

    Drivers::VGA::Write("Thread created, yielding now...\n");

    Drivers::VGA::Write("Main thread back!\n");

    Drivers::VGA::Write("CLI Count: ");
    Drivers::VGA::Write(Kernel::Interrupts::block_interrupt_counter());
    Drivers::VGA::Write("\n");

    assert(Kernel::Interrupts::block_interrupt_counter() == 0);


    sleep(1);

    Drivers::VGA::Write("Main() done sleeping!\n");

    assert(thread2_marker == 1);



    Kernel::context_t* context = new Kernel::context_t;
    if (!Kernel::save_context(context))
    {
        Drivers::VGA::Write("Path 1 Executing...\n");
        Kernel::load_context(context);
    }
    Drivers::VGA::Write("Path 2 Executing...\n");
    

    std::cout << "Multithreading and context tests completed." << std::endl;
}

void ktest_disks()
{

    Drivers::IDE::IDEDevice* _dev = Drivers::IDE::IDEDevice::Get(IDE_PRIMARY, IDE_SLAVE);
    assert(_dev);
    assert(_dev->get_type() == Drivers::IDE::Interface::ATA);

    std::cout << "Device Model: \"" << _dev->get_model() << "\"" << std::endl;

    Drivers::ATA::ATADevice* dev = (Drivers::ATA::ATADevice*)_dev;
    assert(dev->supports_lba());

    uint8_t buf[512];

    for (int i = 0; i < 512; ++i)
    {
        buf[i] = 0;
    }

    bool nonZero = false;

    for (int i = 0; i < 512; ++i)
    {
        nonZero = (nonZero || (buf[i] != 0));
    }

    assert(!nonZero);

    int result = dev->read_sector(0, buf);
    assert(result == 0);

    std::cout << "[510]: " << (void*)buf[510] << "\n[511]: " << (void*)buf[511] << std::endl;

    nonZero = false;

    for (int i = 0; i < 512; ++i)
    {
        nonZero = (nonZero || (buf[i] != 0));
    }

    assert(nonZero);

    uint32_t* buf2 = reinterpret_cast<uint32_t*>(buf);

    for (int i = 0; i < 512 / sizeof(uint32_t); ++i)
    {
        if (buf2[i] != 0)
        {
            std::cout << "LBA[" << (void*)(i * sizeof(uint32_t)) << "]: " << (void*)buf2[i] << std::endl;
        }
        
    }

    // // Drivers::VGA::Write("Initializing ATA Devices...\n");
    // // //Drivers::ATA::Device::Initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
    // // Drivers::ATA::Device::Initialize();
    // // ASSERT(Drivers::ATA::Device::Initialized());
    // // Drivers::VGA::Write("ATA Initialized.\n");
    // // Drivers::VGA::Write("Device 0 Present? ");
    // // Drivers::VGA::Write(Drivers::ATA::Device::Devices[0].reserved != 0);
    // // Drivers::VGA::Write("\n");
    
    // // for (int i = 0; i < 4; ++i)
    // // {
    // //     if (Drivers::ATA::Device::Devices[i].reserved != 0)
    // //     {
    // //         Drivers::VGA::Write("Drive: ");
    // //         Drivers::VGA::Write(i);
    // //         Drivers::VGA::Write("\n");
    // //         Drivers::VGA::Write("Drive Size: ");
    // //         Drivers::VGA::Write(Drivers::ATA::Device::Devices[i].size);
    // //         Drivers::VGA::Write("\n");
    // //         Drivers::VGA::Write("Model: '");
    // //         Drivers::VGA::Write(Drivers::ATA::Device::Devices[i].model);
    // //         Drivers::VGA::Write("'\n");
    // //         Drivers::VGA::Write("Signature: ");
    // //         Drivers::VGA::Write((void*)(addr_t)Drivers::ATA::Device::Devices[i].signature);
    // //         Drivers::VGA::Write("\n");

    // //         // for (int j = 0; j < 40; ++j)
    // //         // {
    // //         //     if (Drivers::ATA::Device::Devices[i].model[j] != 0)
    // //         //     {
    // //         //         Drivers::VGA::Write((unsigned int)Drivers::ATA::Device::Devices[i].model[j]);
    // //         //         Drivers::VGA::Write(", ");
    // //         //     }
    // //         //     else
    // //         //     {
    // //         //         break;
    // //         //     }
    // //         // }
    // //         // Drivers::VGA::Write("\n");
    // //     }
    // // }

    // // std::cout << "Testing SATA PCI scan..." << std::endl;
    // // Drivers::AHCI::AHCIDrive::foo();



    // std::cout << "Testing ATA Drivers..." << std::endl;
    
    // Drivers::ATA::Device::Initialize();
    // Drivers::ATA::Device pm(ATA_PRIMARY, ATA_MASTER);
    // if (pm.reserved)
    // {
    //     // std::cout << "Model: ";
    //     //out.flush();
    //     //Drivers::VGA::Write(pm.model);
    //     std::cout << "Primary-Master Model: " << pm.model << std::endl;
    // }
    
    // Drivers::ATA::Device ps(ATA_PRIMARY, ATA_SLAVE);
    // if (ps.reserved)
    // {
    //     //out << "\nModel: ";
    //     //out.flush();
    //     // Drivers::VGA::Write(ps.model);
    //     //out << "\n";
    //     //out.flush();
    //     std::cout << "Primary-Slave Model: " << ps.model << std::endl;
    // }


    // Drivers::ATA::Device sm(ATA_SECONDARY, ATA_MASTER);
    // if (sm.reserved)
    // {
    //     //out << "Model: ";
    //     //out.flush();
    //     // Drivers::VGA::Write(sm.model);
    //     std::cout << "Secondary-Master Model: " << sm.model << std::endl;
    // }
    
    
    // Drivers::ATA::Device ss(ATA_SECONDARY, ATA_SLAVE);
    // if (ss.reserved)
    // {
    //     //out << "\nModel: ";
    //     //out.flush();
    //     Drivers::VGA::Write(ss.model);
    //     //out << "\n";
    //     //out.flush();
    //     std::cout << "Secondary-Slave Model: " << ss.model << std::endl;
    // }

    // std::cout << "Testing DMADrive..." << std::endl;
    // Drivers::ATA::DMADrive dmdrive(ATA_SECONDARY, ATA_MASTER);

    // auto sec1 = dmdrive.readSector(1, 512);
    // std::cout << "Returned." << std::endl;
    // uint32_t sec1_1 = *(uint32_t*)sec1;
    // std::cout << (void*)sec1_1 << std::endl;
    // std::cout << (void*)(uint32_t)sec1[511] << std::endl;
    // for (int i = 0; i < 512; ++i)
    // {
    //     if (sec1[i] != 1)
    //     {
    //         std::cout << (uint32_t)sec1[i] << std::endl;
    //     }
    // }

    // free(sec1);


    // std::cout << "Testing PIO ATA Driver..." << std::endl;
    // //Drivers::ATA::Device* pio_dev = new Drivers::ATA::Device(ATA_PRIMARY, ATA_MASTER);
    // {
    //     // pio_dev->init();
    //     // pio_dev->softReset();
    //     // assert(pio_dev->reserved);
    //     // assert(pio_dev->model[40] == '\0');
    //     std::cout << "Using drive with model '" << pm.model << "'" << std::endl;
    //     Drivers::ATA::ATADisk pio_disk(&pm);
    //     unsigned char buf[1024];
    //     int count = pio_disk.read(0, 512, buf);
    //     if (count <= 0)
    //     {
    //         const char* error_string = pio_disk.getError();
    //         assert(error_string != nullptr);
    //         std::cout << "PIO Error: \"" << error_string << "\"" << std::endl;
    //     }

    //     assert(count == 512);
    // }
    // //delete pio_dev;
}