#ifndef INCLUDED_KERNEL_FS_NTFS_TYPES_H
#define INCLUDED_KERNEL_FS_NTFS_TYPES_H

#include <Common.h>
#include <Support/PP.h>
#include <kernel/Filesystem/Devices/Signature.h>

#ifdef __cplusplus
#include <Utils/EnumOperators.hh>
#endif

#include "boot_sector.h"

START_NS(Kernel)
START_NS(FS)
C_CODE


#define NTFS_CHK_SZ(T, N) \
IF_CPP(static_assert( sizeof( \
ntfs_##T \
 ) == N ))

#define NTFS_FLAGS(N, T) \
IF_CPP( \
C_END \
ENUM_OPS( \
ntfs_##N,\
 T) \
C_CODE )

typedef enum
{
	NTFS_FILE = 0x454c4946,
	NTFS_INDEX = 0x58444e49,
	NTFS_HOLE = 0x454c4f48,
	NTFS_RSTR = 0x52545352,
	NTFS_RCRD = 0x44524352,
	NTFS_CHKD = 0x444b4843,
	NTFS_BAD = 0x44414142,
	NTFS_EMPTY = 0xffffffff,
	
}
ntfs_record_type;

IF_CPP(static_assert(sizeof(ntfs_record_type) == 4));

NTFS_CHK_SZ(record_type, 4);


typedef struct
{
	ntfs_record_type type_magic;
	uint16_t usa_off;
	uint16_t usa_count;
}
__attribute__((__packed__))
ntfs_record_t;

NTFS_CHK_SZ(record_t, 8);

typedef enum
{
	NTFS_MFT_REC_IN_USE = 0x1,
	NTFS_MFT_REC_IS_DIR = 0x2,
	NTFS_MFT_REC_IS_4 = 0x4,
	NTFS_MFT_REC_IS_VIEW_INDEX = 0x8,
	
	________NTFS_MFT_RECORD_FLAGS_IGNORE = 0xFFFF
}
__attribute__((__packed__))
ntfs_mft_record_flags;

NTFS_CHK_SZ(mft_record_flags, 2);
NTFS_FLAGS(mft_record_flags, uint16_t);



typedef enum
{
	NTFS_FILE_MFT = 0,
	NTFS_FILE_MFT_MIRROR = 1,
	NTFS_FILE_LOG_FILE = 2,
	NTFS_FILE_VOLUME = 3,
	NTFS_FILE_ATTR_DEF = 4,
	NTFS_FILE_ROOT = 5,
	NTFS_FILE_BITMAP = 6,
	NTFS_FILE_BOOT = 7,
	NTFS_FILE_BAD_CLUSTERS = 8,
	NTFS_FILE_SECURE = 9,
	NTFS_FILE_UPCASE = 10,
	NTFS_FILE_EXTEND = 11,
	NTFS_FILE_RESERVED_1 = 12,
	NTFS_FILE_RESERVED_2 = 13,
	NTFS_FILE_RESERVED_3 = 14,
	NTFS_FILE_MFT_DATA = 15,
	NTFS_FILE_FIRST_USER = 16,
}
ntfs_system_files;


typedef struct
{
	ntfs_record_type type_magic;
	uint16_t usa_off;
	uint16_t usa_count;
	uint64_t lsn;
	uint16_t seq_num;
	uint16_t link_count;
	uint16_t attrs_off;
	ntfs_mft_record_flags flags;
	uint32_t bytes_used;
	uint32_t bytes_allocated;
	uint64_t base_mft_record;
	uint16_t next_attr_inst;
	uint16_t reserved;
	uint32_t mft_record_num;
	
}
__attribute__((__packed__))
ntfs_mft_record_t;

NTFS_CHK_SZ(mft_record_t, 48);

IF_CPP(static_assert(sizeof(ntfs_mft_record_t) == 48));


typedef uint16_t ntfs_char_t;


typedef enum
{
	NTFS_ATTR_TYPE_UNUSED = 0x0,
	NTFS_ATTR_TYPE_STD_INFO = 0x10,
	NTFS_ATTR_TYPE_ATTR_LIST = 0x20,
	NTFS_ATTR_TYPE_FILE_NAME = 0x30,
	NTFS_ATTR_TYPE_OBJECT_ID = 0x40,
	NTFS_ATTR_TYPE_SECURITY_DESC = 0x50,
	NTFS_ATTR_TYPE_VOLUME_NAME = 0x60,
	NTFS_ATTR_TYPE_VOLUME_INFO = 0x70,
	NTFS_ATTR_TYPE_DATA = 0x80,
	NTFS_ATTR_TYPE_INDEX_ROOT = 0x90,
	NTFS_ATTR_TYPE_INDEX_ALLOC = 0xA0,
	NTFS_ATTR_TYPE_BITMAP = 0xB0,
	NTFS_ATTR_TYPE_REPARSE_POINT = 0xC0,
	NTFS_ATTR_TYPE_EA_INFO = 0xD0,
	NTFS_ATTR_TYPE_EA = 0xE0,
	NTFS_ATTR_TYPE_PROPERTY_SET = 0xF0,
	NTFS_ATTR_TYPE_LOGGED_UTIL_STREAM = 0x100,
	NTFS_ATTR_TYPE_USER_DEF_1 = 0x1000,
	NTFS_ATTR_TYPE_END = 0xFFFFFFFF,
	
}
ntfs_attr_type;

NTFS_CHK_SZ(attr_type, 4);

typedef enum
{
	// TODO
}
ntfs_collation_rule;

NTFS_CHK_SZ(collation_rule, 4);

typedef enum
{
	// TODO
}
ntfs_attr_def_flags;

NTFS_CHK_SZ(attr_def_flags, 4);

typedef struct
{
	ntfs_char_t name[0x40];
	ntfs_attr_type type;
	uint32_t display_rule;
	ntfs_collation_rule collation_rule;
	ntfs_attr_def_flags flags;
	uint64_t min_size;
	uint64_t max_size;
	
}
__attribute__((__packed__))
ntfs_attr_def_t;

NTFS_CHK_SZ(attr_def_t, 0xA0);

IF_CPP(static_assert(sizeof(ntfs_attr_def_t) == 0xa0));

typedef enum
{
	NTFS_ATTR_COMPRESSED = 0x0001,
	NTFS_ATTR_COMPRESSION_MASK = 0x00FF,
	NTFS_ATTR_ENCRYPTED = 0x4000,
	NTFS_ATTR_SPARSE = 0x8000,
}
__attribute__((__packed__))
ntfs_attr_flags;

NTFS_CHK_SZ(attr_flags, 2);
NTFS_FLAGS(attr_flags, uint16_t);


typedef enum
{
	NTFS_ATTR_RESIDENT_INDEXED = 0x1,
}
__attribute__((__packed__))
ntfs_resident_attr_flags;

NTFS_CHK_SZ(resident_attr_flags, 1);

typedef struct
{
	ntfs_attr_type type;
	uint32_t length;
	uint8_t non_resident;
	uint8_t name_len;
	uint16_t name_off;
	ntfs_attr_flags flags;
	uint16_t instance;
	union
	{
		struct
		{
			uint32_t value_len;
			uint16_t value_off;
			ntfs_resident_attr_flags resident_flags;
			uint8_t reserved1;
			void* resident_end[0];
		}
		__attribute__((__packed__));
		
		struct
		{
			int64_t lowest_vcn;
			int64_t highest_vcn;
			uint16_t mapping_pairs_offset;
			uint8_t compression_unit;
			uint8_t reserved2[5];
			int64_t allocated_size;
			int64_t data_size;
			int64_t initialized_size;
			void* non_resident_end[0];
			int64_t compressed_size;
			void* compressed_end[0];
		}
		__attribute__((__packed__));
	}
	__attribute__((__packed__));
}
__attribute__((__packed__))
ntfs_attr_record_t;

NTFS_CHK_SZ(attr_record_t, 72);



typedef enum
{
	NTFS_ATTR_FILE_READONLY = 0x1,
	NTFS_ATTR_FILE_HIDDEN = 0x2,
	NTFS_ATTR_FILE_SYSTEM = 0x4,
	NTFS_ATTR_FILE_DIR = 0x10,
	NTFS_ATTR_FILE_ARCHIVE = 0x20,
	NTFS_ATTR_FILE_DEVICE = 0x40,
	NTFS_ATTR_FILE_NORMAL = 0x80,
	NTFS_ATTR_FILE_TEMPORARY = 0x100,
	NTFS_ATTR_FILE_SPARSE = 0x200,
	NTFS_ATTR_FILE_REPARSE_POINT = 0x400,
	NTFS_ATTR_FILE_COMPRESSED = 0x800,
	NTFS_ATTR_FILE_OFFLINE = 0x1000,
	NTFS_ATTR_FILE_NOT_CONTENT_INDEXED = 0x2000,
	NTFS_ATTR_FILE_ENCRYPTED = 0x4000,
	
	NTFS_ATTR_FILE_VALID_FLAGS = 0x7fb7,
	NTFS_ATTR_FILE_VALID_SET_FLAGS = 0x31A7,
	NTFS_ATTR_FILE_I30_INDEX_PRESENT = 0x10000000,
	NTFS_ATTR_FILE_VIEW_INDEX_PRESENT = 0x20000000,
}
__attribute__((__packed__))
ntfs_file_attr_flags;

NTFS_CHK_SZ(file_attr_flags, 4);
NTFS_FLAGS(file_attr_flags, uint32_t);


typedef struct
{
	int64_t creation_dt;
	int64_t last_data_change_dt;
	int64_t last_mft_change_dt;
	int64_t last_access_dt;
	ntfs_file_attr_flags file_attrs;
	
	union
	{
		struct
		{
			uint8_t reserved1[12];
			void* v1_end[0];
		}
		__attribute__((__packed__));
		
		struct
		{
			uint32_t max_allowed_file_versions;
			uint32_t version_num;
			uint32_t class_ID;
			uint32_t owner_ID;
			uint32_t security_ID;
			uint64_t quota_charged;
			uint64_t last_update_seq_num;
			void* v3_end[0];
		}
		__attribute__((__packed__));
	}
	__attribute__((__packed__));
}
__attribute__((__packed__))
ntfs_stdinfo_t;

NTFS_CHK_SZ(stdinfo_t, 72);


typedef struct
{
	ntfs_attr_type type;
	uint16_t length;
	uint8_t name_len;
	uint8_t name_off;
	uint64_t lowest_vcn;
	uint64_t mft_reference;
	uint16_t instance;
	ntfs_char_t name[];
}
__attribute__((__packed__))
ntfs_attrlist_entry_t;

NTFS_CHK_SZ(attrlist_entry_t, 26);


// Layout.h - Ln 1054
#define NTFS_MAX_NAME_LEN 255

typedef enum
{
	NTFS_FILENAME_POSIX = 0x00,
	NTFS_FILENAME_WIN32 = 0x01,
	NTFS_FILENAME_DOS = 0x02,
	NTFS_FILENAME_WIN32_AND_DOS = 0x03,
}
__attribute__((__packed__))
ntfs_filename_flags;

NTFS_CHK_SZ(filename_flags, 1);


typedef struct
{
	uint64_t parent_dir;
	int64_t creation_dt;
	int64_t last_data_change_dt;
	int64_t last_mft_change_dt;
	int64_t last_access_dt;
	int64_t allocated_size;
	int64_t data_size;
	ntfs_file_attr_flags file_attrs;
	
	union
	{
		struct
		{
			uint16_t packed_ea_size;
			uint16_t reserved1;
			
		}
		__attribute__((__packed__));
		uint32_t reparse_point_tag;
	}
	__attribute__((__packed__));
	uint8_t filename_len;
	ntfs_filename_flags filename_type;
	ntfs_char_t filename[];
}
__attribute__((__packed__))
ntfs_filename_attr_t;

NTFS_CHK_SZ(filename_attr_t, 66);



typedef struct
{
	uint32_t data_1;
	uint16_t data_2;
	uint16_t data_3;
	uint8_t data_4[8];
}
__attribute__((__packed__))
ntfs_guid_t;


NTFS_CHK_SZ(guid_t, 16);

#define PACKED __attribute__((__packed__))

typedef struct
{
	uint64_t mft_ref;
	union
	{
		struct
		{
			ntfs_guid_t birth_volume_id;
			ntfs_guid_t birth_object_id;
			ntfs_guid_t domain_id;
		}
		PACKED;
		uint8_t extended_info[48];
	}
	PACKED;
}
PACKED
ntfs_obj_id_index_data_t;

NTFS_CHK_SZ(obj_id_index_data_t, 56);


#define NTFS_DEF(N, D, SZ) \
typedef \
D \
PACKED \
ntfs_##N; \
\
NTFS_CHK_SZ(N, SZ);


NTFS_DEF(object_id_attr_t, struct
{
	ntfs_guid_t object_id;
	union
	{
		struct
		{
			ntfs_guid_t birth_volume_id;
			ntfs_guid_t birth_object_id;
			ntfs_guid_t domain_id;
		}
		PACKED;
		uint8_t extended_info[48];
	}
	PACKED;
	
}, 64);



typedef enum
{
	// TODO
}
ntfs_relative_identifier;



// Layout.h - Ln 1363
NTFS_DEF(sid_identifier_auth_t, union
{
	struct
	{
		uint16_t high_part;
		uint32_t low_part;
	}
	PACKED;
	
	uint8_t value[6];
	
}, 6);



NTFS_DEF(sid_t, struct
{
	uint8_t rev;
	uint8_t sub_auth_count;
	ntfs_sid_identifier_auth_t identifier_auth;
	union
	{
		struct
		{
			uint32_t ____authority1;
			uint32_t ____authorityN[];
		}
		PACKED;
		struct
		{
			uint32_t authority[]; // At least 1
		}
		PACKED;
	}
	PACKED;
	
}, 12);


typedef enum
{
	NTFS_SID_REVISION = 0x1,
	NTFS_SID_MAX_SUB_AUTHS = 15,
	NTFS_SID_REC_SUB_AUTHS = 1,
}
ntfs_sid_const;




typedef enum
{
	NTFS_ACE_ACCESS_MIN_MS_TYPE = 0x0,
	NTFS_ACE_ACCESS_ALLOWED_TYPE = 0x0,
	NTFS_ACE_ACCESS_DENIED_TYPE = 0x1,
	NTFS_ACE_SYSTEM_AUDIT_TYPE = 0x2,
	NTFS_ACE_SYSTEM_ALARM_TYPE = 0x3,
	NTFS_ACE_ACCESS_MAX_MS_V2_TYPE = 0x3,
	NTFS_ACE_ACCESS_ALLOWED_COMPOUND_TYPE = 0x4,
	NTFS_ACE_ACCESS_MAX_MS_V3_TYPE = 0x4,
	
	
	NTFS_ACE_ACCESS_OBJECT_MIN_MS_TYPE = 0x5,
	NTFS_ACE_ACCESS_OBJECT_ALLOWED_TYPE = 0x5,
	NTFS_ACE_ACCESS_OBJECT_DENIED_TYPE = 0x6,
	NTFS_ACE_SYSTEM_OBJECT_AUDIT_TYPE = 0x7,
	NTFS_ACE_SYSTEM_OBJECT_ALARM_TYPE = 0x8,
	NTFS_ACE_ACCESS_OBJECT_MAX_MS_TYPE = 0x8,
	
	
	NTFS_ACE_ACCESS_MAX_MS_V4_TYPE = 0x8,
	
	
	NTFS_ACE_ACCESS_MAX_MS_TYPE = 0x8,
	
}
PACKED
ntfs_ace_types;

NTFS_CHK_SZ(ace_types, 1);



typedef enum
{
	NTFS_ACE_OBJECT_INHERIT = 0x1,
	NTFS_ACE_CONTAINER_INHERIT = 0x2,
	NTFS_ACE_NO_PROPAGATE_INHERIT = 0x4,
	NTFS_ACE_ONLY_INHERIT = 0x8,
	NTFS_ACE_INHERITED = 0x10,
	
	
	NTFS_VALID_ACE_FLAGS = 0x1F,
	
	
	NTFS_ACE_SUCCESSFUL_ACCESS = 0x40,
	NTFS_ACE_FAILED_ACCESS = 0x80,
	
}
PACKED
ntfs_ace_flags;

NTFS_CHK_SZ(ace_flags, 1);



NTFS_DEF(ace_head_t, struct
{
	ntfs_ace_types type;
	ntfs_ace_flags flags;
	uint16_t size;
}, 4);



typedef enum
{
	NTFS_ACCESS_FILE_READ_DATA = 0x1,
	NTFS_ACCESS_FILE_LIST_DIR = 0x2,
	
	// TODO
	// ...
	
	NTFS_ACCESS_DELETE = 0x010000,
	
	// TODO
	// ...
	
	
	NTFS_ACCESS_GENERIC_ALL = 0x10000000,
	
	// TODO
	// ...
}
PACKED
ntfs_access_mask;

NTFS_CHK_SZ(access_mask, 4);




NTFS_DEF(generic_mapping_t, struct
{
	ntfs_access_mask generic_read;
	ntfs_access_mask generic_write;
	ntfs_access_mask generic_execute;
	ntfs_access_mask generic_all;
}, 16);



typedef struct
{
	ntfs_ace_types type;
	ntfs_ace_flags flags;
	uint16_t size;
	ntfs_access_mask mask;
	ntfs_sid_t sid;
}
PACKED
ntfs_access_allowed_ace_t,
ntfs_access_denied_ace_t,
ntfs_system_audit_ace_t,
ntfs_system_alarm_ace_t;

NTFS_CHK_SZ(access_allowed_ace_t, 20);



typedef enum
{
	// TODO
	NTFS_OBJECT_ACE_PRESENT = (uint32_t)1,
	NTFS_OBJECT_ACE_INHERITED_PRESENT = (uint32_t)2,
}
ntfs_object_ace_flags;

NTFS_CHK_SZ(object_ace_flags, 4);




typedef struct
{
	ntfs_ace_types type;
	ntfs_ace_flags flags;
	uint16_t size;
	ntfs_access_mask mask;
	ntfs_object_ace_flags object_flags;
	ntfs_guid_t object_type;
	ntfs_guid_t inherited_object_type;
	ntfs_sid_t sid;
}
PACKED
ntfs_access_allowed_object_ace_t,
ntfs_access_denied_object_ace_t,
ntfs_system_audit_object_ace_t,
ntfs_system_alarm_object_ace_t;

NTFS_CHK_SZ(access_allowed_object_ace_t, 56);


NTFS_DEF(acl_t, struct
{
	uint8_t rev;
	uint8_t ___ignore1;
	uint16_t size;
	uint16_t ace_count;
	uint16_t ___ignore2;
}, 8);


typedef enum
{
	// Current
	NTFS_ACL_REVISION = 2,
	NTFS_ACL_REVISION_DS = 4,
	
	// Old revisions
	NTFS_ACL_REVISION1 = 1,
	NTFS_ACL_REVISION_MIN = 2,
	NTFS_ACL_REVISION2 = 2,
	NTFS_ACL_REVISION3 = 3,
	NTFS_ACL_REVISION4 = 4,
	NTFS_ACL_REVISION_MAX = 4,
}
ntfs_acl_const;




typedef enum
{
	NTFS_SE_OWNER_DEFAULTED = 0x0001,
	
	// TODO
	// ...
	
	
	NTFS_SE_SELF_RELATIVE = 0x8000,
}
PACKED
ntfs_security_desc_ctrl;

NTFS_CHK_SZ(security_desc_ctrl, 2);




NTFS_DEF(security_desc_rel_t, struct
{
	uint8_t rev;
	uint8_t ___ignore1;
	ntfs_security_desc_ctrl control;
	uint32_t owner;
	uint32_t group;
	uint32_t sys_acl_off;
	uint32_t discretion_acl_off;
}, 20);



NTFS_DEF(sd_head_t, struct
{
	uint32_t hash;
	uint32_t security_id;
	uint64_t offset;
	uint32_t length;
}, 20);


NTFS_DEF(sdhead_index_data_t, struct
{
	uint32_t hash;
	uint32_t security_id;
	uint64_t offset;
	uint32_t length;
	union
	{
		uint32_t II;
		uint32_t reserved;
	}
	PACKED;
}, 24);

#ifdef __cplusplus



#define NTFS_DEFPP(N, D, SZ, ...) \
struct ntfs_##N PP_IF_ARGS(:, __VA_ARGS__) __VA_ARGS__ \
D \
PACKED; \
NTFS_CHK_SZ(N , SZ);

#else

#define NTFS_DEFPP(...)

#endif


NTFS_DEFPP(sds_entry_t, {
	
	ntfs_security_desc_rel_t sid;
	
}, 40, ntfs_sd_head_t);





NTFS_DEF(sii_index_k, struct
{
	uint32_t security_id;
}, 4);


NTFS_DEF(sdh_index_k, struct
{
	uint32_t hash;
	uint32_t security_id;
}, 8);


typedef struct
{
	ntfs_char_t name[];
} PACKED
ntfs_volume_name_t;


typedef enum
{
	NTFS_VOLUME_DIRTY = 0x0001,
	NTFS_VOLUME_RESIZE_LOG_FILE = 0x0002,
	
	// TODO
	// ...
	
	NTFS_VOLUME_MODIFIED_BY_CHKDSK = 0x8000,
	NTFS_VOLUME_FLAGS_MASK = 0xC03F,
}
PACKED
ntfs_volume_flags;

NTFS_CHK_SZ(volume_flags, 2);



NTFS_DEF(volume_info_t, struct
{
	uint64_t ___reserved1;
	uint8_t major_ver;
	uint8_t minor_ver;
	ntfs_volume_flags flags;
}, 12);


typedef enum
{
	NTFS_INDEX_SMALL = 0x0,
	
	// TODO
	// ...
}
PACKED
ntfs_index_head_flags;

NTFS_CHK_SZ(index_head_flags, 1);



NTFS_DEF(index_head_t, struct
{
	uint32_t entries_off;
	uint32_t index_len;
	uint32_t allocated_size;
	ntfs_index_head_flags flags;
	
	uint8_t __reserved1[3];
}, 16);


NTFS_DEF(index_root_t, struct
{
	ntfs_attr_type type;
	ntfs_collation_rule collation_rules;
	uint32_t index_block_size;
	int8_t clusters_per_index_block;
	uint8_t __reserved1[3];
	ntfs_index_head_t index;
}, 32);


NTFS_DEF(index_block_t, struct
{
	ntfs_record_type magic; // = "INDX"
	uint16_t usa_off;
	uint16_t usa_count;
	uint64_t lsn;
	uint64_t index_block_vcn;
	ntfs_index_head_t index;
}, 40);


NTFS_DEF(reparse_index_k, struct
{
	uint32_t reparse_tag;
	uint64_t file_id;
}, 12);


// Layout.h - Ln 2175
typedef enum
{
	NTFS_QUOTA_FLAG_DEFAULT_LIMITS = 0x00000001,
	
	// TODO
	// ...
	
	NTFS_QUOTA_FLAG_PENDING_DELETES = 0x00000800,
	
	________NTFS_QUOTA_FLAG_IGNORE = 0xFFFFFFFF,
}
PACKED
ntfs_quota_flags;

NTFS_CHK_SZ(quota_flags, 4);




typedef enum
{
	NTFS_QUOTA_INVALID_ID = 0x0,
	NTFS_QUOTA_DEFAULTS_ID = 0x1,
	NTFS_QUOTA_FIRST_USER_ID = 0x100,
	
	
	
	________NTFS_QUOTA_PREDEF_ID_IGNORE = 0xFFFFFFFF,
}
PACKED
ntfs_predef_owner_id;

NTFS_CHK_SZ(predef_owner_id, 4);



typedef enum
{
	NTFS_INDEX_ENTRY_NODE = 0x1,
	NTFS_INDEX_ENTRY_END = 0x2,
	
	
	
	________NTFS_INDEX_ENTRY_FLAGS_IGNORE = 0xFFFF,
}
PACKED
ntfs_index_entry_flags;

NTFS_CHK_SZ(index_entry_flags, 2);


NTFS_DEF(index_entry_head_t, struct
{
	union
	{
		uint64_t indexed_file;
		struct
		{
			uint16_t data_off;
			uint16_t data_len;
			uint32_t ___reserved1;
		}
		PACKED;
	}
	PACKED;
	
	uint16_t length;
	uint16_t key_length;
	ntfs_index_entry_flags flags;
	uint16_t ___reserved2;
}, 16);



NTFS_DEFPP(index_entry_t, {
	
	union
	{
		ntfs_filename_attr_t filename;
		ntfs_sii_index_k sii;
		ntfs_sdh_index_k sdh;
		ntfs_guid_t object_id;
		ntfs_reparse_index_k reparse;
		ntfs_sid_t sid;
		uint32_t owner_id;
	}
	PACKED key;
	
	
	
}, sizeof(ntfs_index_entry_head_t) + 66, ntfs_index_entry_head_t);


NTFS_DEF(bitmap_attr_t, struct
{
	uint8_t bitmap[];
}, 0);



NTFS_DEF(reparse_point_t, struct
{
	uint32_t tag;
	uint16_t data_len;
	uint16_t ___reserved1;
	uint8_t data[];
}, 8);


NTFS_DEF(ext_attr_info_t, struct
{
	uint16_t length;
	uint16_t need_ea_count;
	uint32_t ea_query_len;
}, 8);


typedef enum
{
	NTFS_NEED_EXT_ATTR = 0x80,
}
PACKED
ntfs_ext_attr_flags;

NTFS_CHK_SZ(ext_attr_flags, 1);



NTFS_DEF(ext_attr_t, struct
{
	uint32_t next_entry_off;
	ntfs_ext_attr_flags flags;
	uint8_t name_len;
	uint16_t value_len;
	uint8_t name[];
	uint8_t value[];
}, 8);



NTFS_DEF(efs_attr_head_t, struct
{
	uint32_t length;
	uint32_t state;
	uint32_t version;
	uint32_t crypto_api_version;
	uint8_t ____unknown1[16];
	uint8_t ____unknown2[16];
	uint8_t ____unknown3[16];
	uint32_t ddf_arr_off;
	uint32_t drf_arr_off;
	uint32_t ____reserved1;
}, 76);


NTFS_DEF(efs_df_arr_head_t, struct
{
	uint32_t df_count;
}, 4);


NTFS_DEF(efs_df_head_t, struct
{
	uint32_t df_len;
	uint32_t cred_head_off;
	uint32_t fek_size;
	uint32_t fek_off;
	uint32_t ____unknown1;
}, 20);






NTFS_DEF(efs_df_cred_head_t, struct
{
	uint32_t cred_len;
	uint32_t sid_off;
	
	uint32_t type;
	// 0x1 = CryptoAPI
	// 0x2 = Unexpected/invalid type
	// 0x3 = Certificate thumbprint
	// Any Other Value = Unknown
	
	
	union
	{
		struct // CryptoAPI
		{
			uint32_t container_name_off;
			uint32_t provider_name_off;
			uint32_t public_key_blob_off;
			uint32_t public_key_blob_sz;
		}
		PACKED crypto;
		
		struct // Cert Thumbprint
		{
			uint32_t head_sz;
			uint32_t head_off;
			uint32_t ____unknown1;
			uint32_t ____unknown2;
		}
		PACKED cert_thumb;
	}
	PACKED;
	
}, 28);



typedef enum
{
	NTFS_INTX_SYMLINK_TYPE = 0x014B4E4C78746E49ULL,
	
	NTFS_INTX_CHAR_DEV_TYPE = 0x0052484378746E49ULL,
	
	NTFS_INTX_BLOCK_DEV_TYPE = 0x004B4C4278746E49ULL,
}
PACKED
ntfs_intx_file_type;

NTFS_CHK_SZ(intx_file_type, 8);



NTFS_DEF(intx_file_t, struct ___ntfs_intx_file_t
{
	ntfs_intx_file_type magic;
	union
	{
		dev64_t dev;
		
		struct
		{
			ntfs_char_t target[];
		}
		PACKED;
	}
	PACKED;
}, 24);
















#undef NTFS_DEFPP
#undef NTFS_DEF
#undef PACKED
#undef NTFS_CHK_SZ


C_END
END_NS // FS
END_NS // Kernel


#endif