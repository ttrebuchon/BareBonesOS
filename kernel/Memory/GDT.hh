#ifndef INCLUDED_GDT_HH
#define INCLUDED_GDT_HH

#include <Common.h>

namespace Kernel { namespace Memory
{
    enum class GDTType
    {
        Data = 0x2,
        Code = 0xA
    };

    struct GDTPtr
    {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    extern "C" void flush_gdt(GDTPtr*);

    struct GDTEntry
    {
        public:
        uint16_t    limit_low;
        uint16_t    base_low;
        uint8_t     base_middle;
        uint8_t     _type : 4;           // 2 = Data,    10 = Code
        uint8_t     desc_type : 1;      // Always 1
        uint8_t     privilege : 2;      // 0 = Kernel,  3 = User
        uint8_t     present : 1;

        uint8_t     len : 4;            // Segment Length 19:16
        uint8_t     system : 1;         // Always 0
        uint8_t     zero : 1;           // Always 0
        uint8_t     operand_size : 1;   // 0 = 16-bit,  1 = 32-bit
        uint8_t     granularity : 1;    // 0 = Byte,    1 = KByte
        
        uint8_t     base_high;


        GDTEntry();

        uint32_t limit() const;
        void limit(uint32_t);

        uint32_t base() const;
        void base(uint32_t);

        GDTType type() const;
        void type(GDTType);

        static GDTPtr Pointer;
        static void Flush();

    } __attribute__((packed));

    static_assert(sizeof(GDTEntry) == 8);


    void init_gdt();

    extern GDTEntry* gdt_table;
}
}


#endif