#include "GDT.hh"
#include <Debug.h>


namespace Kernel { namespace Memory {

    GDTEntry gdt_table[5];

    void init_gdt()
    {
        ASSERT(*reinterpret_cast<uint64_t*>(&gdt_table[0]) == 0);
        gdt_table[1].limit(0xFFFFFFF);
        gdt_table[1].base(0);
        gdt_table[1].present = 1;
        gdt_table[1].privilege = 0;
        gdt_table[1].type(Kernel::Memory::GDTType::Code);
        gdt_table[1].granularity = 1;
        gdt_table[1].operand_size = 1;

        gdt_table[2].limit(0xFFFFFFF);
        gdt_table[2].base(0);
        gdt_table[2].present = 1;
        gdt_table[2].privilege = 0;
        gdt_table[2].type(Kernel::Memory::GDTType::Data);
        gdt_table[2].granularity = 1;
        gdt_table[2].operand_size = 1;

        gdt_table[3].limit(0xFFFFFFF);
        gdt_table[3].base(0);
        gdt_table[3].present = 1;
        gdt_table[3].privilege = 3;
        gdt_table[3].type(Kernel::Memory::GDTType::Code);
        gdt_table[3].granularity = 1;
        gdt_table[3].operand_size = 1;

        gdt_table[4].limit(0xFFFFFFF);
        gdt_table[4].base(0);
        gdt_table[4].present = 1;
        gdt_table[4].privilege = 3;
        gdt_table[4].type(Kernel::Memory::GDTType::Data);
        gdt_table[4].granularity = 1;
        gdt_table[4].operand_size = 1;

        Drivers::VGA::Write(reinterpret_cast<uint8_t*>(&gdt_table[1])[5]);
        Drivers::VGA::Write("\n");
        ASSERT(reinterpret_cast<uint8_t*>(&gdt_table[1])[5] == 0x9A);

        Kernel::Memory::GDTEntry::Pointer.limit = (sizeof(Kernel::Memory::GDTEntry)*5 - 1);
        Kernel::Memory::GDTEntry::Pointer.base = (uint32_t)&gdt_table;
        Kernel::Memory::GDTEntry::Flush();
    }



}
}

