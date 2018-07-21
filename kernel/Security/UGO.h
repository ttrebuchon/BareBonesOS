#ifndef INCLUDED_KERNEL_SECURITY_OGO_H
#define INCLUDED_KERNEL_SECURITY_OGO_H

#include <Common.h>

START_NS(Kernel)
C_CODE


typedef enum UGO_permission_set : uint16_t
{
	PERMISSIONS_OTHER = 0x0,
	PERMISSIONS_GROUP = 0x8,
	PERMISSIONS_USER = 0x40,
} UGO_permission_set_t;

typedef enum UGO_right : uint16_t
{
	PERMISSION_EXECUTE = 0x1,
	PERMISSION_WRITE = 0x2,
	PERMISSION_READ = 0x4,
}
UGO_right_t;


// "UGO" = "user", "group", "other"
typedef struct UGO_permissions
{
	uint16_t other_execute : 1;
	uint16_t other_write : 1;
	uint16_t other_read : 1;
	
	uint16_t group_execute : 1;
	uint16_t group_write : 1;
	uint16_t group_read : 1;
	
	uint16_t user_execute : 1;
	uint16_t user_write : 1;
	uint16_t user_read : 1;
	
	uint16_t sticky : 1;
	uint16_t set_GID : 1;
	uint16_t set_UID : 1;
	
}
UGO_t;

C_END




#ifdef __cplusplus
	static_assert(sizeof(UGO_t) == sizeof(uint16_t));
	static_assert(alignof(UGO_t) == alignof(uint16_t));
#endif




END_NS // Kernel

#endif