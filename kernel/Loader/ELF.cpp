#include "ELF.h"

namespace Kernel
{
    extern "C"
    int ELF_verify_magic(const void* buf)
    {
        if (!buf)
        {
            return false;
        }

        const unsigned char* magic = reinterpret_cast<const unsigned char*>(buf);

        // Check each byte one at a time, in case we're looking at nonsense data to avoid
        // a page fault and to interact with it as little as possible
        if (magic[0] != 0x7F)
        {
            return false;
        }
        else if (magic[1] != 'E')
        {
            return false;
        }
        else if (magic[2] != 'L')
        {
            return false;
        }
        else if (magic[3] != 'F')
        {
            return false;
        }

        return true;
    }


    template <class Header_t>
    int ELF_parse_info(const Header_t* header, int* is_32_bit, int* is_little_endian, 
                    int* is_x86, int* ELF_version, int* ELF_kind)
    {
        if (is_32_bit)
        {
            if (header->bitness == 1)
            {
                *is_32_bit = 1;
            }
            else
            {
                *is_32_bit = 0;
            }
        }

        if (is_little_endian)
        {
            if (header->endianess == 1)
            {
                *is_little_endian = 1;
            }
            else if (header->endianess == 2)
            {
                *is_little_endian = 0;
            }
            else
            {
                return -1;
            }
        }

        if (is_x86)
        {
            if (header->instruction_set == ELF_ARCH_X86)
            {
                *is_x86 = 1;
            }
            else
            {
                *is_x86 = 0;
            }
        }

        if (ELF_version)
        {
            *ELF_version = header->ELF_version;
        }

        if (ELF_kind)
        {
            *ELF_kind = header->kind;
        }

        return 1;
    }


    extern "C"
    int ELF_parse_info(const void* header, int* is_32_bit, int* is_little_endian, 
                    int* is_x86, int* ELF_version, int* ELF_kind)
    {
        if (!ELF_verify_magic(header))
        {
            return -1;
        }

        const ELF32Header_t* naive_header = reinterpret_cast<const ELF32Header_t*>(header);

        if (naive_header->bitness == 1)
        {
            return ELF_parse_info<ELF32Header_t>(naive_header, is_32_bit, is_little_endian, is_x86, ELF_version, ELF_kind);
        }
        else if (naive_header->bitness == 2)
        {
            return ELF_parse_info<ELF64Header_t>((const ELF64Header_t*)naive_header, is_32_bit, is_little_endian, is_x86, ELF_version, ELF_kind);
        }
        else
        {
            return -1;
        }
    }
}