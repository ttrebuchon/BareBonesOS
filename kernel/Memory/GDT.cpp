#include "GDT.hh"


namespace Kernel { namespace Memory {

    GDTPtr GDTEntry::Pointer {0, 0};

    GDTEntry::GDTEntry()  :
                        limit_low(0),
                        base_low(0),
                        base_middle(0),
                        present(0),
                        privilege(0),
                        desc_type(0),
                        _type(0),
                        granularity(0),
                        operand_size(0),
                        zero(0),
                        system(0),
                        base_high(0)
    {

    }

    uint32_t GDTEntry::limit() const
    {
        return limit_low;
    }

    void GDTEntry::limit(uint32_t val)
    {
        present = 1;
        desc_type = 1;
        limit_low = val & 0xFFFF;
        len = ((val >> 16) & 0x0F);
    }

    uint32_t GDTEntry::base() const
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(this);
        uint32_t value = 0;
        uint8_t* valPtr = reinterpret_cast<uint8_t*>(&value);
        valPtr[3] = base_high;
        valPtr[2] = base_middle;
        valPtr[1] = 0xFF & base_low;
        valPtr[0] = 0xFF00 & base_low;
        return value;
    }

    void GDTEntry::base(uint32_t val)
    {
        present = 1;
        desc_type = 1;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(this);
        base_low = (val & 0xFF) << 8;
        base_low |= (val >> 8) & 0xFF;
        base_middle = (val >> 16) & 0xFF;
        base_high = (val >> 24) & 0xFF;
    }

    GDTType GDTEntry::type() const
    {
        return (GDTType)_type;
    }

    void GDTEntry::type(GDTType val)
    {
        present = 1;
        desc_type = 1;
        _type = (uint8_t)val;
    }


    void GDTEntry::Flush()
    {
        flush_gdt(&Pointer);
    }

}
}
