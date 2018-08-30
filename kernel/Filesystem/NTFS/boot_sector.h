#ifndef INCLUDED_KERNEL_FS_NTFS_BOOT_SECTOR_H
#define INCLUDED_KERNEL_FS_NTFS_BOOT_SECTOR_H

#include <Common.h>

START_NS(Kernel)
START_NS(FS)
C_CODE



typedef struct
{
	
	uint8_t jmp[3];
	
	// = "NTFS"
	union
	{
		uint8_t OEM_ID[8];
		uint64_t OEM_ID_value;
	};
	uint16_t bytes_per_sec;
	uint8_t sec_per_cluster;
	uint16_t reserved_sectors; // Always 0
	uint8_t unused1[3];
	uint8_t unused2[2];
	uint8_t media_type;
	uint8_t unused3[2];
	uint16_t sec_per_track;
	uint16_t head_count;
	uint32_t hidden_sectors;
	uint8_t unused4[4];
	uint8_t unused5[4];
	uint64_t sec_count;
	uint64_t MFT_cluster_no;
	uint64_t MFT_mirror_cluster_no;
	int8_t clusters_per_mft_record;
	uint8_t unused6[3];
	int8_t clusters_per_index_record;
	uint8_t unused7[3];
	uint8_t serial_no[8];
	uint32_t checksum;
	uint8_t boostrap_code[426];
	uint8_t boot_flag[2]; // 0xAA55
	
}
__attribute__((__packed__))
ntfs_boot_t;

IF_CPP(static_assert(sizeof(ntfs_boot_t) == 512));

C_END
END_NS // FS
END_NS // Kernel


#endif