#ifndef INCLUDED_KERNEL_SECURITY_GROUP_H
#define INCLUDED_KERNEL_SECURITY_GROUP_H

#include <Common.h>
#include "Types.h"

START_NS(Kernel)
C_CODE

typedef struct _group_t
{
	gid_t id;
}
group_t;

#define NOGROUP ((gid_t)-1)


typedef struct groupset
{
	void* data;
}
groupset_t;


int groupset_init(groupset_t*) _NOEX __attribute__((nonnull(1)));

int groupset_destroy(groupset_t*) _NOEX __attribute__((nonnull(1)));

gid_t get_groupset_id(const groupset_t*, uint32_t index) _NOEX __attribute__((nonnull(1)));

int groupset_has_id(const groupset_t*, gid_t id) _NOEX __attribute__((nonnull(1)));

int groupset_add_id(groupset_t*, gid_t id) _NOEX __attribute__((nonnull(1)));

int groupset_get_groups(const groupset_t*, int count, gid_t*) _NOEX __attribute__((nonnull(1)));

int groupset_get_groups_from(const groupset_t*, int count, gid_t*, int start_index) _NOEX __attribute__((nonnull(1)));

size_t groupset_count(const groupset_t*) _NOEX __attribute__((nonnull(1)));

int groupset_copy(groupset_t*, const groupset_t*) _NOEX __attribute__((nonnull(1, 2)));


C_END
END_NS // Kernel

#endif