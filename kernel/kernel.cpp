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
#include <kernel/Loader/ELF.h>

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
void ktest_ELF();


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
	ktest_ELF();



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

	
	assert(dev->role == IDE_SLAVE);
	if (dev->role == IDE_SLAVE)
	{
		uint32_t* buf2 = reinterpret_cast<uint32_t*>(buf);
		for (int i = 0; i < 512 / sizeof(uint32_t); ++i)
		{
			assert(buf2[i] == i*4);
			//assert(buf2[i] == 0xDEADBABA);
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

	delete _dev;
}

void ktest_ELF()
{
	std::cout << "Entry Count: " << boot::mboot->raw()->u.elf_sec.num << std::endl;
	std::cout << "Entry Size: " << boot::mboot->raw()->u.elf_sec.size << std::endl;
	assert(false);	// TODO

	Drivers::IDE::IDEDevice* _dev = Drivers::IDE::IDEDevice::Get(IDE_PRIMARY, IDE_SLAVE);
	assert(_dev);
	assert(_dev->get_type() == Drivers::IDE::Interface::ATA);

	std::cout << "Device Model: \"" << _dev->get_model() << "\"" << std::endl;

	Drivers::ATA::ATADevice* dev = (Drivers::ATA::ATADevice*)_dev;
	assert(dev->supports_lba());

	uint8_t first_sector[512];

	int res = dev->read_sector(1, first_sector);
	if (res != 0)
	{
		std::cout << "Error: " << res << std::endl;
	}
	assert(res == 0);

	uint64_t lib_size = *reinterpret_cast<uint64_t*>(first_sector);
	assert(lib_size > 0);

	uint64_t sectors_size = lib_size;

	uint64_t end_addr = lib_size + 512 + sizeof(uint64_t);
	if (end_addr % 512 != 0)
	{
		end_addr += 512 - (end_addr % 512);
	}

	const size_t sector_count = (end_addr / 512) - 1;

	uint8_t* diskbuf = new uint8_t[end_addr - 512];

	assert(end_addr - 512 == sector_count*512);

	uint8_t* libbuf = diskbuf + sizeof(uint64_t);

	res = dev->read(1, diskbuf, sector_count);
	assert(res == 0);
	

	Kernel::ELF32Object obj(libbuf);

	std::cout << "Program Header Entries: " << obj.program_header_count() << std::endl;

	std::cout << "Section Entries: " << obj.section_header_count() << std::endl;
	std::cout << "Section Entry Size: " << obj.section_header_entry_size() << std::endl;

	std::cout << "ELF File Kind: ";
	switch (obj.kind())
	{
		case ELF_KIND_UNKNOWN:
			std::cout << "Unknown" << std::endl;
			assert(false);
		
		case ELF_KIND_RELOC:
			std::cout << "Relocatable";
			break;
		
		case ELF_KIND_SHARED:
			std::cout << "Shared Object";
			break;
		
		case ELF_KIND_EXEC:
			std::cout << "Executable";
			break;

		case ELF_KIND_CORE:
			std::cout << "Core";
			break;
	}
	std::cout << std::endl;



	for (int i = 1; i < obj.section_header_count(); ++i)
	{
		auto header = obj.section_header(i);
		assert(header);

		std::cout << "Header " << i << " Name: '" << obj.section_name(i) << "' Type: '" << header->type << std::endl;
	}


	auto dynsym = obj.section_header(".dynsym");
	assert(dynsym);

	std::cout << "Symbol Table Count: " << obj.symbol_table_count() << std::endl;
	assert(obj.symbol_table_count() > 0);
	assert(obj.symbol_table_count() == 2);
	auto symbol_tab1 = obj.symbol_table(0);
	assert(symbol_tab1);
	auto sym_tab1_name = symbol_tab1->table_name();
	assert(sym_tab1_name);
	std::cout << "First Symbol Table Name: " << sym_tab1_name << std::endl;
	std::cout << "Table Symbol Count: " << symbol_tab1->symbol_count() << std::endl;

	for (int i = 1; i < symbol_tab1->symbol_count(); ++i)
	{
		auto sym = symbol_tab1->symbol(i);
		assert(sym);

		if (sym->type() == ELF_SYM_FUNC)
		{
			std::cout << "Function ";
		}

		std::cout << "Symbol " << i;

		if (sym->name())
		{
			std::cout << ": '" << sym->name() << "'";
		}
		else
		{
			std::cout << ": [NONE]";
		}

		std::cout << " value: " << (void*)sym->value() << std::endl;

		if (sym->name())
		{
			if (strcmp(sym->name(), "foo") == 0)
			{
				if (sym->type() == ELF_SYM_FUNC)
				{
					std::cout << "SHNDX: " << (void*)(addr_t)sym->shndx() << std::endl;
					std::cout << "Raw Value: " << (void*)(addr_t)sym->raw_value() << std::endl;
					assert(sym->shndx() != 0);
					auto target_section = obj.section_header(sym->shndx());
					assert(target_section);
					if (target_section->name_string_index != 0)
					{
						std::cout << "Target Section Name: " << obj.name_lookup(target_section) << std::endl;
					}
					std::cout << "Target Section Offset: " << (void*)(addr_t)target_section->offset << std::endl;
					


					const char* man_data = (const char*)libbuf;
					const char* tmp = man_data;
					while (*reinterpret_cast<const uint32_t*>(tmp) != 0xDEADBABA)
					{
						++tmp;
					}

					std::cout << "Manually Found 0xDEADBABA:\nOffset to file: " << (void*)(tmp - man_data) << std::endl;
					std::cout << std::endl << "Symbol Value Relative to file: " << (void*)((const char*)sym->value() - man_data) << std::endl;

					std::cout << "Number of symbols: " << symbol_tab1->symbol_count() << std::endl;

					const char* data = (const char*)sym->value();
					assert(data);

					tmp = data;
					while (*reinterpret_cast<const uint32_t*>(tmp) != 0xDEADBABA)
					{
						++tmp;
					}

					std::cout << "Found 0xDEADBABA:\nOffset from value: " << (tmp - data);
					std::cout << "\nTotal Offset: " << ((const uint8_t*)tmp - libbuf);
					std::cout << "\nSymbol Value: " << sym->value() << std::endl;

					auto foo = (uint32_t(*)())sym->value();
					assert(foo() == 0xDEADBABA);

					std::cout << "foo() == " << (void*)foo() << std::endl;


					break;
				}
			}
		}
	}

	auto rela_PLT_section = obj.section_header(".rel.plt");
	assert(rela_PLT_section);

	auto rela_PLT = obj.relocation_table(".rel.plt");
	assert(rela_PLT);
	std::cout << rela_PLT->table_name() << std::endl;

	assert(rela_PLT->link() != 0);
	assert(obj.section_header(rela_PLT->link()));
	std::cout << obj.section_header(rela_PLT->link())->type << std::endl;
	{
		auto link_section = obj.section_header(rela_PLT->link());
		assert(link_section);
		if (link_section->name_string_index != 0)
		{
			std::cout << obj.name_lookup(link_section) << std::endl;
			std::cout << "Index is: " << rela_PLT->link() << std::endl;
		}
		else
		{
			std::cout << "Section has no name, index is: " << rela_PLT->link() << std::endl;
		}

		for (int i = 0; i < obj.symbol_table_count(); ++i)
		{
			auto stbl = obj.symbol_table(i);
			assert(stbl);
			if (stbl->get() == obj.section_header(rela_PLT->link()))
			{
				std::cout << "Found at symbol table index " << i << std::endl;
			}
			else
			{
				std::cout << i << ": Not correct table table [";
				if (stbl->table_name())
				{
					std::cout << stbl->table_name();
				}
				else
				{
					std::cout << "NoName";
				}
				std::cout << "] (" << stbl->get()->type << ")" << std::endl;
			}
		}
		std::cout << "Done trying to find symbol table." << std::endl;
	}

	for (int i = 1; i < rela_PLT->count(); ++i)
	{
		auto reloc = rela_PLT->reloc(i);
		assert(reloc);
		assert(reloc->info_symbol() > 0);
		auto sym = reloc->symbol();
		assert(sym);
		std::cout << "Index " << i << ": '";
		if (sym->name())
		{
			std::cout << sym->name();
		}
		else
		{
			std::cout << "NoName";
		}
		std::cout << "'" << std::endl;
	}

	


	auto GOT = obj.symbol("_GLOBAL_OFFSET_TABLE_");
	assert(GOT);
	assert(GOT->type() == ELF_SYM_OBJECT);
	std::cout << "GOT File-Relative Value: " << (void*)(GOT->value() - (addr_t)libbuf) << std::endl;




	void** linker_callback = (void**)(GOT->value() + 2*sizeof(addr_t));
	void** linker_callback_arg = (void**)(GOT->value() + sizeof(addr_t));

	*linker_callback_arg = nullptr;
	*linker_callback = (void*)(void(*)(void*))[](void* arg) -> void
	{
		std::cout << "CALLBACK HIT!" << std::endl;
		while (true) ;
	};

	const Kernel::ELF32Reloc* foo_reloc = nullptr;
	const Kernel::ELF32Symbol* foo_sym = nullptr;

	for (int i = 0; i < obj.relocation_table_count() && !foo_sym; ++i)
	{
		auto tbl = obj.relocation_table(i);
		assert(tbl);
		for (int j = 1; j < tbl->count(); ++j)
		{
			auto reloc = tbl->reloc(j);
			assert(reloc);
			auto sym = reloc->symbol();
			assert(sym);
			if (strcmp(sym->name(), "foo") == 0)
			{
				foo_sym = sym;
				foo_reloc = reloc;
				break;
			}
		}
	}

	assert(foo_reloc);
	assert(foo_sym);

	auto got_plt = obj.section_header(".got.plt");
	assert(got_plt);
	std::cout << ".got.plt Offset: " << (void*)got_plt->offset << std::endl;
	std::cout << ".got.plt Addr: " << (void*)got_plt->address << std::endl;

	assert(got_plt->flags & ELF_SEC_ATTR_ALLOC);

	//memcpy((void*)((addr_t)libbuf + got_plt->address), (void*)((addr_t)libbuf + got_plt->offset), got_plt->size);


	std::cout << "foo_sym value (Relative to file): " << (void*)(foo_sym->value() - (addr_t)libbuf) << std::endl;
	assert(foo_reloc->target_loc());
	std::cout << "foo_reloc target (Relative to file): " << (void*)((addr_t)foo_reloc->target_loc() - (addr_t)libbuf) << std::endl;

	// std::cout << "ELF_RELOC_386_32 style value (Relative to file): " << (void*)(*((addr_t*)foo_reloc->target_loc()) + (addr_t)foo_reloc->symbol()->value() - (addr_t)libbuf) << std::endl;
	// std::cout << "ELF_RELOC_386_PC32 style value (Relative to file): " << (void*)(*((addr_t*)foo_reloc->target_loc()) + (addr_t)foo_reloc->symbol()->value() - (addr_t)libbuf - (addr_t)foo_reloc->target_loc()) << std::endl;

	std::cout << "foo_reloc value (Relative to file): " << (void*)((addr_t)foo_reloc->reloc_value() - (addr_t)libbuf) << std::endl;

	std::cout << "Old Value of foo_reloc target (Relative to file): " << (void*)((addr_t)*(void**)foo_reloc->target_loc() - (addr_t)libbuf) << std::endl;
	std::cout << "Old Value of foo_reloc target: " << *(void**)foo_reloc->target_loc() << std::endl;

	std::cout << "Offset-ed target_loc: " << *(void**)((addr_t)foo_reloc->target_loc() - got_plt->address + got_plt->offset) << std::endl;
	std::cout << "^Address (Relative to file): " << (void*)((addr_t)foo_reloc->target_loc() - got_plt->address + got_plt->offset - (addr_t)libbuf) << std::endl;


	//*(void**)foo_reloc->target_loc() = (void*)foo_reloc->reloc_value();

	for (int i = 1; i < obj.relocation_table_count(); ++i)
	{
		auto rtbl = obj.relocation_table(i);
		assert(rtbl);
		for (int j = 1; j < rtbl->count(); ++j)
		{
			auto rloc = rtbl->reloc(j);
			assert(rloc);
			*(void**)rloc->target_loc() = (void*)rloc->reloc_value();
		}
	}



	auto foo = (uint32_t(*)())(void*)foo_sym->value();
	assert(foo() == 0xDEADBABA);


	auto foo2_sym = obj.symbol("foo2");
	assert(foo2_sym);

	std::cout << "foo2_sym value (Relative to file): "<< (void*)(foo2_sym->value() - (addr_t)libbuf) << std::endl;

	std::cout << "Manually Calculated (Relative): " << (void*)(0x400 + 0x12fc) << std::endl;
	std::cout << "Symbol Value Calculated (Relative): " << (void*)(GOT->value() - (addr_t)libbuf) << std::endl;
	assert((void*)(0x400 + 0x12fc + libbuf) == (void*)(GOT->value()));

	auto foo2 = (uint32_t(*)())foo2_sym->value();
	assert(foo2);

	std::cout << "Here comes the segfault bois!" << std::endl;
	assert(foo2() == 0xDEADBABA);
	std::cout << "WTF" << std::endl;

	for (int i = 1; i < obj.symbol_table_count(); ++i)
	{
		auto stbl = obj.symbol_table(i);
		for (int j = 1; j < stbl->symbol_count(); ++j)
		{
			auto s = stbl->symbol(j);
			assert(s);
			if (s->name())
			{
				std::cout << s->name() << std::endl;
			}
		}
	}

	auto new_sym = obj.symbol("_Znwm");
	assert(new_sym);
	std::cout << "new_sym value: " << (void*)new_sym->value() << std::endl;
	std::cout << "new_sym binding: " << new_sym->binding() << std::endl;

	auto get_poly = (void*(*)())obj.symbol("get_poly")->value();
	assert(get_poly);
	std::cout << "Here comes the clusterfuck." << std::endl;
	auto get_poly_res = get_poly();
	assert(get_poly_res);

	// const unsigned char* section_table = reinterpret_cast<const unsigned char*>(obj.section_header_table());

	// char* entry_buf = new char[obj.section_header_entry_size() + 1];
	// memcpy(entry_buf, section_table + (obj.section_header_entry_size()*obj.section_names_index()), obj.section_header_entry_size());
	// entry_buf[obj.section_header_entry_size()] = 0;

	// char tmp[2];
	// tmp[1] = 0;

	// std::cout << "Section Names Index: " << obj.section_names_index() << std::endl;

	// std::cout << "Some Entry: " << entry_buf << std::endl;
	// for (int i = 0; i < obj.section_header_entry_size(); ++i)
	// {
	// 	tmp[0] = entry_buf[i];
	// 	std::cout << tmp;
	// }
	// std::cout << std::endl;
	// for (int i = 0; i < obj.section_header_entry_size(); ++i)
	// {
	// 	std::cout << " " << (void*) entry_buf[i] << " ";
	// }
	// std::cout << std::endl;

	// bool found_94 = false;
	// for (int i = 0; i < lib_size; ++i)
	// {
	// 	if (libbuf[i] == 0x94)
	// 	{
	// 		std::cout << "Found 94 at offset " << i << std::endl;
	// 		found_94 = true;
	// 	}
	// }

	// std::cout << "Table offset: " << section_table - (unsigned char*)libbuf << std::endl;

	// assert(found_94);


	// std::cout << obj.name_table() << std::endl;


	// for (int i = 0; i < obj.section_header_count(); ++i)
	// {
	// 	auto section_header = obj.section_header(i);
	// 	assert(section_header);

	// 	if (section_header->type == ELF_SEC_SYMTABLE)
	// 	{
	// 		for (int j = 1; j < (section_header->size / section_header->entry_size); ++j)
	// 		{
	// 			auto sym = obj.symbol(i, j);
	// 			if (sym)
	// 			{
	// 				assert(sym);

	// 				auto name = obj.name_lookup(sym);
	// 				if (name)
	// 				{
	// 					std::cout << name << std::endl;
	// 				}
	// 				else
	// 				{
	// 					std::cout << "[No Name]" << std::endl;
	// 				} 
	// 			}          
	// 		}
	// 	}
	// }


	// delete entry_buf;
	delete[] diskbuf;
	delete dev;
}