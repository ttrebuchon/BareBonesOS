#ifndef INCLUDED_KERNEL_SCHEDULING_PROCESS_H
#define INCLUDED_KERNEL_SCHEDULING_PROCESS_H

#include <Common.h>
#include "filesystem_context.h"

START_NS(Kernel)
#ifdef __cplusplus
namespace FS
{
	class DirectoryNode_v;
}
#endif
C_CODE

typedef struct process_resources presources_t;

typedef struct process_info
{
	pid_t id;
	uid_t user;
	struct filesystem_context fs_context;
	presources_t* resources;
}
process_info_t;




typedef struct process
{
	struct context* context;
	int status;
	struct
	{
		pthread_t* ids;
		size_t id_count;
	} threads;
	union
	{
		struct process_info info;
		/*struct
		{
			pid_t id;
			uid_t user;
			struct filesystem_context fs_context;
		};*/
	};
}
process_t;

int create_new_process_uid(struct process**, uid_t);
int create_new_process_uid_fs(struct process**, uid_t, const struct filesystem_context*);
int destroy_process(struct process*);


C_END
END_NS // Kernel

#endif