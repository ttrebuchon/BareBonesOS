#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

#include <stddef.h>
#include <stdint.h>
#include "c_cpp.h"

#ifdef __cplusplus
namespace Kernel {
#endif


struct gdt_entry
{
    uint16_t    limit_low;
    uint16_t    base_low;
    uint8_t     base_middle;
    uint8_t     access;
    uint8_t     granularity;
    uint8_t     base_high;

    #ifdef __cplusplus

    void setLimit(unsigned int lim)
    {
        if (lim > 65536)
        {
            lim = lim >> 12;
            granularity = 0xC0;
        }
        else
        {
            granularity = 0x40;
        }

        uint8_t* ptr = reinterpret_cast<uint8_t*>(&this->limit_low);
        ptr[0] = lim & 0xFF;
        ptr[1] = (lim >> 8) & 0xFF;
        granularity |= (lim >> 16) & 0xF;
    }

    uint16_t getLimit() const
    {
        return limit_low;
    }

    void setBase(uint32_t base)
    {
        uint8_t* ptr = reinterpret_cast<uint8_t*>(this);
        ptr[2] = base & 0xFF;
        ptr[3] = (base >> 8) & 0xFF;
        ptr[4] = (base >> 16) & 0xFF;
        ptr[7] = (base >> 24) & 0xFF;
    }

    uint32_t getBase() const
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(this);
        uint32_t value = 0;
        uint8_t* valPtr = reinterpret_cast<uint8_t*>(&value);
        valPtr[3] = ptr[7];
        valPtr[2] = ptr[4];
        valPtr[1] = ptr[3];
        valPtr[0] = ptr[2];
        return value;
    }

    void setType(const uint8_t t)
    {
        this->access = t;
    }

    uint8_t getType() const
    {
        return access;
    }

    static const gdt_entry nullEntry()
    {
        return gdt_entry{ 0, 0, 0, 0, 0, 0 };
    }

    static void loadTable(gdt_entry*, const unsigned int entry_count);
    static void reloadSegments();

    #endif

} __attribute__((packed));


EXTERN_C void loadGdt(struct gdt_entry*, const unsigned int entry_count);
EXTERN_C void reloadGdtSegments();

#ifdef __cplusplus
void gdt_entry::loadTable(gdt_entry* table, const unsigned int entry_count)
{
    loadGdt(table, entry_count*sizeof(gdt_entry));
}
#endif

#ifdef __cplusplus
void gdt_entry::reloadSegments()
{
    reloadGdtSegments();
}
#endif

#ifdef __cplusplus
}
#endif



extern "C"
void foo();

#endif