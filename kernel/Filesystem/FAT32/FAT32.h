#ifndef INCLUDED_FAT32_H
#define INCLUDED_FAT32_H

#include <Common.h>

#ifdef __cplusplus

#include <kernel/Filesystem/Filesystem.hh>
#include <kernel/Filesystem/ReadOnlyFactory.hh>

#endif

START_NS(Kernel)
START_NS(FS)

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
	uint32_t root_cluster_num;
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
	char name[8]; // - 0x00
	char ext[3]; // - 0x08
	uint8_t attr; // 0x0B
	uint8_t user_attr; // - 0x0C
	
	char undelete; // - 0x0D
	uint16_t time_create; // - 0x0E
	uint16_t date_create; // - 0x10
	uint16_t date_access; // - 0x12
	uint16_t cluster_high; // - 0x14
	
	uint16_t time_modify; // - 0x16
	uint16_t date_modify; // - 0x18
	uint16_t cluster_low; // - 0x1A
	uint32_t filesize; // - 0x1C
	
	
} __attribute__((__packed__))
FAT32DirEnt_t;

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_LABEL 0x08
#define FAT_ATTR_SUB_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_DEVICE 0x40
#define FAT_ATTR_RESERVED 0x80








typedef struct
{
	uint8_t zero; // 0x00
	uint8_t ignore[31];
}
FAT32DirClusterEntry_EOD_t;

typedef struct
{
	uint8_t unused; // 0xE5
	uint8_t ignore[31];
}
FAT32DirClusterEntry_Unused_t;


typedef enum 
{
	FAT32_DIR_CLUS_EOD,
	FAT32_DIR_CLUS_UNUSED,
	FAT32_DIR_CLUS_LONG_NAME,
	FAT32_DIR_CLUS_NORMAL,
}
FAT32DirClusterEntryType;

typedef struct
{
	union
	{
		FAT32DirClusterEntry_EOD_t eod;
		FAT32DirClusterEntry_Unused_t unused;
		FAT32DirEnt_t entry;
	};
	
	#ifdef __cplusplus
	FAT32DirClusterEntryType entry_type() const;
	#endif
}
FAT32DirClusterEntry_t;

FAT32DirClusterEntryType dir_cluster_entry_type(const FAT32DirClusterEntry_t*);



IF_CPP(static_assert(sizeof(FAT32DirClusterEntry_t) == 32));


typedef struct
{
	uint32_t cluster_number;
	
	// TODO
}
FAT32Cluster_t;


typedef struct
{
	//size_t count; // (sector_size() / 32)
	FAT32DirClusterEntry_t entries[16];
	// ^^ Only if sector_size() == 512....
}
FAT32DirCluster_t;


#define FAT32_CLUSTER_EOF 0x0FFFFFF8
#define FAT32_CLUSTER_BAD 0x0FFFFFF7





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
	
	uint32_t* table;
	uint32_t table_entry_count;
	
}
FAT32_t;

typedef struct
{
	uint32_t value : 28;
	
	IF_CPP(private:)
	
	uint32_t reserved : 2;
	
	IF_CPP(public:)
	
	IF_CPP_ELSE(bool, uint32_t) eof : 1;
	IF_CPP_ELSE(bool, uint32_t) bad_cluster : 1;
	
}
FAT32ClusterNumber_t;


C_END

#ifdef __cplusplus


class FAT32 : public Kernel::FS::Filesystem
{
	public:
	
	protected:
	Drivers::Disk* disk;
	FAT32_t fs;
	ReadOnlyFactory _factory; // TODO
	
	uint32_t get_cluster_lba(uint32_t cluster) const noexcept;
	
	public:
	constexpr static const char* ExtensionSeparator = ".";
	
	FAT32(Drivers::Disk&, int partition = -1) noexcept;
	
	uint16_t sectors_per_cluster() const noexcept;
	uint16_t sector_size() const noexcept;
	uint32_t cluster_size() const noexcept
	{ return ((uint32_t)sector_size()) * sectors_per_cluster(); }
	addr_t cluster_lba(uint32_t cluster) const noexcept;
	int read_cluster_sector(unsigned char* buf, uint32_t cluster, uint32_t sector) const noexcept;
	
	int read_cluster(void* buf, uint32_t cluster) const noexcept;
	
	FAT32ClusterNumber_t next_cluster(uint32_t active_cluster) const;
	
	FAT32ClusterNumber_t next_cluster(const FAT32ClusterNumber_t active) const;
	
	
	int read_directory_cluster(uint32_t number, FAT32DirClusterEntry_t* buffer, size_t buffer_count) const;
	
	FAT32ClusterNumber_t root_directory_cluster() const noexcept;
	
	uint32_t next_free_cluster(uint32_t start_index = 0) const;
	
	
	virtual Node* root() const noexcept override;
	
	virtual ReadOnlyFactory& factory() noexcept override
	{
		return _factory;
	}
};

#endif

END_NS //} FS
END_NS //} Kernel


#endif