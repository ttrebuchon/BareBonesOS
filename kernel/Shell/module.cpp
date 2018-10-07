#include "module.hh"
#include "shell.hh"
#include <kernel/module.h>
#include <kernel/Memory/Allocators/virtual.hh>
#include <Utils/string>
#include <kernel/Scheduling/process.h>
#include <kernel/Filesystem.hh>

namespace Kernel
{
	
	ShellModule::ShellModule(const allocator_type& a, bool self_allocated) : alloc(a), self_allocated(self_allocated)
	{
		
	}
	
	ShellModule::~ShellModule()
	{
		
		
	}
	
	
	constexpr static Shell_StartOpts default_opts = SHELL_START_DEFAULT | SHELL_START_ALLOC_PROCESS_INFO;
	
	Shell* ShellModule::start(uid_t usr)
	{
		allocator_type::template rebind<Shell>::other shell_alloc(alloc);
		
		auto shell = shell_alloc.allocate(1);
		assert(shell);
		if (!shell)
		{
			return nullptr;
		}
		
		// TODO: Create process
		process_t* proc = nullptr;
		
		
		new (shell) Shell(alloc, proc, default_opts);
		
		return shell;
	}
	
	Shell* ShellModule::start(uid_t usr, FS::DirectoryNode_v* cwd)
	{
		return start(usr, nullptr, cwd);
	}
	
	Shell* ShellModule::start(uid_t usr, FS::Filesystem* fs, FS::DirectoryNode_v* cwd)
	{
		if (!cwd)
		{
			if (fs)
			{
				auto n_root = fs->root();
				if (n_root)
				{
					auto root = n_root->as_directory();
					if (root)
					{
						return start(usr, fs, root);
					}
				}
			}
			return start(usr);
		}
		
		allocator_type::template rebind<Shell>::other shell_alloc(alloc);
		
		auto shell = shell_alloc.allocate(1);
		assert(shell);
		if (!shell)
		{
			return nullptr;
		}
		
		// TODO: Create process
		process_t* proc = nullptr;
		
		new (shell) Shell(alloc, proc, default_opts | SHELL_START_WAIT_CWD);
		
		assert(shell->fs_ctx);
		shell->set_cwd(cwd);
		shell->set_root_filesystem(fs);
		
		return shell;
	}
	
	
	
	Shell* ShellModule::start(uid_t usr, FS::Filesystem* fs)
	{
		if (fs)
		{
			auto n_root = fs->root();
			if (n_root)
			{
				auto root = n_root->as_directory();
				if (root)
				{
					return start(usr, fs, root);
				}
			}
		}
		
		return start(usr);
	}
	
	int ShellModule::stop(Shell* shell)
	{
		allocator_type::template rebind<Shell>::other shell_alloc(alloc);
		
		shell->~Shell();
		
		// TODO: Process cleanup
		
		shell_alloc.deallocate(shell, 1);
		
		return 0;
	}
	
	
	
	
	
	
	
	
	
	
	struct mod_info* ShellModule::module_info(struct mod_info* info) noexcept
	{
		info->setup_object_size = sizeof(ShellModule);
		info->open = nullptr;
		info->close = nullptr;
		
		const char name[] = "shell module";
		
		size_t i;
		for (i = 0; i < sizeof(name); ++i)
		{
			info->name[i] = name[i];
		}
		
		for (; i < MAX_MODULE_NAME_LENGTH; ++i)
		{
			info->name[i] = '\0';
		}
		
		// TODO
		
		info->open = [](auto obj, auto args)
		{
			return (void*)ShellModule::module_open(obj, args);
		};
		
		info->close = [](void* m)
		{
			ShellModule* mod = (ShellModule*)m;
			if (mod->self_allocated)
			{
				Memory::virtual_allocator<uint8_t>::template rebind<ShellModule>::other mod_alloc(mod->alloc);
				mod->~ShellModule();
				mod_alloc.deallocate(mod, 1);
			}
			else
			{
				mod->~ShellModule();
			}
		};
		
		return info;
	}
	
	
	
	
	ShellModule* ShellModule::module_open(void* obj, const struct mod_init_args* args)
	{
		bool self_alloc = false;
		ShellModule* mod = (ShellModule*)obj;
		if (!mod)
		{
			auto alloc = args->heap_alloc;
			assert(alloc);
			
			Memory::virtual_allocator<uint8_t>::template rebind<ShellModule>::other mod_alloc(*alloc);
			mod = mod_alloc.allocate(1);
			self_alloc = true;
		}
		
		assert(args->heap_alloc);
		new (mod) ShellModule(*args->heap_alloc, self_alloc);
		
		
		return mod;
	}
}