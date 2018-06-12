#ifndef INCLUDED_FAT32_H
#define INCLUDED_FAT32_H

#include <Common.h>

#ifdef __cplusplus

#include <kernel/Filesystem/Filesystem.hh>

#endif

START_NS(Kernel)
START_NS(Filesystem)

C_CODE

#define FAT32_BAD_SECTOR 0x0FFFFFF7
#define FAT32_PARTITION1_LBA_BEGIN 0x1c6




typedef unsigned char FAT32Table_t[512];


typedef struct FATBoot
{
	uint8_t bootjmp[3];
	uint8_t OEM_name[8];
	
	// <- Offset = 11 bytes
	
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	
	// <- Offset = 16 bytes
	
	uint8_t table_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media_type;
	uint16_t table_size_16;
	
	// <- Offset = 24 bytes
	
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;
	
	// <- Offset = 36 bytes
	
} __attribute__((__packed__))
FATBoot_t;

#ifndef __cplusplus
typedef struct FAT32Boot
{
	uint8_t bootjmp[3];
	uint8_t OEM_name[8];
	
	// <- Offset = 11 bytes
	
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	
	// <- Offset = 16 bytes
	
	uint8_t table_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media_type;
	uint16_t table_size_16;
	
	// <- Offset = 24 bytes
	
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;
	
	// <- Offset = 36 bytes
	
	
	uint32_t table_size_32;
	uint16_t extended_flags;
	uint16_t FAT_version;
	uint32_t root_cluster;
	uint16_t FAT_info;
	uint16_t backup_BS_sector;
	uint8_t reserved[2];
	uint8_t drive_number;
	uint8_t reserved2;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fat_type_label[8];
	
	
	// <- Offset = 80 bytes
	
	unsigned char __padding1[370];
	
	// <- Offset = 450 bytes
	
	uint32_t partition1_typecode;
	uint32_t partition1_lba_begin;
	uint32_t partition1_size;
	
	unsigned char __padding2[50];
	
	
	
} __attribute__((__packed__))
FAT32Boot_t;

#else



struct FAT32Boot_t : public FATBoot_t
{
	// <- Offset = 36 bytes
	
	
	uint32_t table_size_32;
	uint16_t extended_flags;
	uint16_t FAT_version;
	uint32_t root_cluster;
	uint16_t FAT_info;
	uint16_t backup_BS_sector;
	uint8_t reserved[2];
	uint8_t drive_number;
	uint8_t reserved2;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fat_type_label[8];
	
	// <- Offset = 80 bytes
	
	unsigned char __padding1[370];
	
	// <- Offset = 450 bytes
	
	uint32_t partition1_typecode;
	uint32_t partition1_lba_begin;
	uint32_t partition1_size;
	
	unsigned char __padding2[50];
	
	
} __attribute__((__packed__));

static_assert(sizeof(FAT32Boot_t) == 512);
static_assert(sizeof(FATBoot_t) == 36);

#endif

typedef struct
{
	uint32_t cluster_number;
	// TODO
}
FAT32Cluster_t;

typedef struct
{
	char name[8];
	char ext[3];
	uint8_t attr;
	uint8_t user_attr;
	
	char undelete;
	uint16_t time_create;
	uint16_t date_create;
	uint16_t date_access;
	uint16_t cluster_high;
	
	uint16_t time_modify;
	uint16_t date_modify;
	uint16_t cluster_low;
	uint32_t filesize;
	
	
} __attribute__((__packed__))
FAT32DirEnt_t;


typedef struct FAT32FS
{
	uint32_t sector_count;
	uint8_t sectors_per_cluster;
	uint32_t cluster_count;
	uint32_t data_sector_count;
	uint32_t lba_begin;
	uint8_t partition;
	uint16_t first_data_sector;
	uint32_t table_size;
	uint16_t first_table_sector;
	
	// First FAT LBA addr
	uint32_t FAT_lba_begin;
	// Addr of first data cluster
	uint32_t cluster_lba_begin;
	
	FAT32Table_t root_table;
	FAT32Boot_t boot;
	
}
FAT32_t;



C_END

#ifdef __cplusplus


class FAT32 : public Kernel::Filesystem::Filesystem
{
	public:
	
	protected:
	Drivers::Disk* disk;
	FAT32_t fs;
	
	uint32_t get_cluster_lba(uint32_t cluster) const noexcept;
	
	public:
	
	FAT32(Drivers::Disk&, int partition = -1) noexcept;
	
	uint16_t sector_size() const noexcept;
	addr_t cluster_lba(uint32_t cluster) const noexcept;
	int read_cluster_sector(unsigned char* buf, uint32_t cluster, uint32_t sector) const noexcept;
	
	
	
};

#endif

END_NS //} Filesystem
END_NS //} Kernel


#endif