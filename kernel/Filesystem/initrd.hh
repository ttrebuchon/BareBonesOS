#ifndef INCLUDED_FILESYSTEM_INITRD_HH
#define INCLUDED_FILESYSTEM_INITRD_HH

#include <Common.h>
#include "Filesystem.hh"
#include "initrd/DirectoryNode.hh"
#include "initrd/Factory.hh"

namespace Kernel { namespace FS {
	
	class Node;
	
	namespace Init_RD
	{
		class DirectoryNode;
	}
	
	
	
	node_ptr<> init_initrd(void* addr);
	
	class InitRD_FS : public Filesystem::Filesystem
	{
		public:
		
		private:
		node_ptr<FS::DirectoryNode_v> _root;
		void* _base;
		Init_RD::Factory _factory;
		
		
		public:
		InitRD_FS(void* base);
		
		virtual node_ptr<> root() const noexcept override
		{
			return _root;
		}
		
		virtual Init_RD::Factory& factory() noexcept override
		{
			return _factory;
		}
		
		#ifdef TESTING
		node_ptr<FileNode_v> create_native_file(const Utils::string& path, const Utils::string& native);
		#endif
	};
	
	
}
}
#endif