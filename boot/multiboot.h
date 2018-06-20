#ifndef INCLUDED_MULTIBOOT_H
#define INCLUDED_MULTIBOOT_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MULTIBOOT_FLAG_MEM     0x001
#define MULTIBOOT_FLAG_DEVICE  0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS    0x008
#define MULTIBOOT_FLAG_AOUT    0x010
#define MULTIBOOT_FLAG_ELF     0x020
#define MULTIBOOT_FLAG_MMAP    0x040
#define MULTIBOOT_FLAG_CONFIG  0x080
#define MULTIBOOT_FLAG_LOADER  0x100
#define MULTIBOOT_FLAG_APM     0x200
#define MULTIBOOT_FLAG_VBE     0x400

typedef struct multiboot_header
{
	
	uint32_t magic;
	
	uint32_t flags;
	
	uint32_t checksum;
	
	// Only valid if 
	// MULTIBOOT_AOUT_KLUDGE is set
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
	
	// Valid if
	// MULTIBOOT_VIDEO_MODE is set
	uint32_t mode_type;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	
	
}__attribute__((packed)) multiboot_header_t;

typedef struct
{
    uint32_t table_size;
    uint32_t str_size;
    uint32_t addr;
    uint32_t reserved;
    
} multiboot_aout_symbol_table_t;

typedef struct
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;

} multiboot_elf_section_header_table_t;


struct multiboot
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    union
    {
        multiboot_aout_symbol_table_t aout_sym;
        multiboot_elf_section_header_table_t elf_sec;
    } u;
    // uint32_t num;
    // uint32_t size;
    // uint32_t addr;
    // uint32_t shndx;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
} __attribute__((packed));

typedef struct multiboot_header multiboot_header_t;


typedef struct multiboot_mmap_item
{
	uint32_t size;
	uint64_t base_addr;
	uint64_t len;
	uint32_t type;
	
	
	
}__attribute__((packed)) multiboot_mmap_t;


typedef struct multiboot_module
{
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t string_addr;
	uint32_t reserved;
}
__attribute__((__packed__))
multiboot_module_t;

#ifdef __cplusplus
}
#endif

#endif