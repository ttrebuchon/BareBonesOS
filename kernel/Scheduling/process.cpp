#include "process.h"
#include "process_resources.h"
#include <Std/errno.h>

#define PROCESS_ALLOCATION_MAGIC 0xABCDEF12




namespace Kernel
{
	struct process_allocation_helper
	{
		uint32_t magic;
		process_t process;
		presources_t resources;
	};
	
	process_allocation_helper* construct_helper()
	{
		auto data = Memory::cross_proc_allocator<process_allocation_helper>().allocate(1);
		new (data) process_allocation_helper();
		data->magic = PROCESS_ALLOCATION_MAGIC;
		data->process.info.resources = &data->resources;
		return data;
	}
	
	void destroy_helper(process_allocation_helper* helper)
	{
		helper->~process_allocation_helper();
		
		Memory::cross_proc_allocator<process_allocation_helper>().deallocate(helper, 1);
	}
	
	extern "C"
	pthread_t get_inc_pid();
	
	extern "C"
	int create_new_process_uid(struct process** ptr, uid_t uid)
	{
		if (!ptr)
		{
			tset_error(EINVAL, "NULL pointer passed to create_new_process_uid: struct process** cannot be NULL.");
			return EINVAL;
		}
		
		auto helper = construct_helper();
		if (!helper)
		{
			*ptr = nullptr;
			tset_error(ENOMEM, "Couldn't allocate new process.");
			return ENOMEM;
		}
		
		*ptr = &helper->process;
		helper->process.info.user = uid;
		
		helper->process.info.id = get_inc_pid();
		
		return 0;
	}
	
	extern "C"
	int create_new_process_uid_fs(struct process** ptr, uid_t uid, const struct filesystem_context* fs_ctx)
	{
		if (!fs_ctx)
		{
			tset_error(EINVAL, "NULL pointer passed to create_new_process_uid_fs: const struct filesystem_context* cannot be NULL.");
			return EINVAL;
		}
		
		
		
		int res = create_new_process_uid(ptr, uid);
		if (res != 0)
		{
			return res;
		}
		
		(*ptr)->info.fs_context.cwd = fs_ctx->cwd;
		(*ptr)->info.fs_context.fs = fs_ctx->fs;
		
		
		return 0;
	}
	
	extern "C"
	int destroy_process(struct process* ptr)
	{
		if (!ptr)
		{
			tset_error(EINVAL, "NULL pointer passed to destroy_process: struct process* cannot be NULL.");
			return EINVAL;
		}
		
		// TODO: Unregister process
		// ...
		
		// TODO: Delete context if not null
		// ...
		
		process_allocation_helper* helper;
		
		helper = (process_allocation_helper*)((char*)ptr-offsetof(process_allocation_helper, process));
		
		if (helper->magic == PROCESS_ALLOCATION_MAGIC)
		{
			destroy_helper(helper);
		}
		else
		{
			assert(NOT_IMPLEMENTED);
		}
		
		return 0;
	}
}