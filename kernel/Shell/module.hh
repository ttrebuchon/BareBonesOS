#ifndef INCLUDED_KERNEL_SHELL_MODULE_HH
#define INCLUDED_KERNEL_SHELL_MODULE_HH

#include <kernel/Memory/Allocators/virtual.hh>

namespace Kernel
{
	namespace FS
	{
		class Filesystem;
		class DirectoryNode_v;
	}
	class ShellModule
	{
		public:
		typedef Memory::virtual_allocator<uint8_t> allocator_type;
		
		private:
		allocator_type alloc;
		bool self_allocated;
		
		public:
		
		ShellModule(const allocator_type&, bool self_allocated);
		~ShellModule();
		
		
		
		
		
		class Shell* start(uid_t);
		class Shell* start(uid_t, FS::DirectoryNode_v* cwd);
		class Shell* start(uid_t, FS::Filesystem*, FS::DirectoryNode_v* cwd);
		class Shell* start(uid_t, FS::Filesystem*);
		int stop(class Shell*);
		
		
		
		
		
		
		
		
		
		
		static struct mod_info* module_info(struct mod_info*) noexcept __attribute__((__nonnull__(1)));
		
		static ShellModule* module_open(void*, const struct mod_init_args*);
	};
	
}

#endif