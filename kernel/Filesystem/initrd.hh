#ifndef INCLUDED_FILESYSTEM_INITRD_HH
#define INCLUDED_FILESYSTEM_INITRD_HH

#include <Common.h>
#include "Filesystem.hh"
#include "initrd/DirectoryNode.hh"
#include "ReadOnlyFactory.hh"

namespace Kernel { namespace FS {
	
	class Node;
	
	namespace Init_RD
	{
		class DirectoryNode;
	}
	
	
	
	Node* init_initrd(void* addr);
	
	class InitRD_FS : public Filesystem::Filesystem
	{
		public:
		
		private:
		Init_RD::DirectoryNode* _root;
		void* _base;
		ReadOnlyFactory _factory;
		
		
		public:
		InitRD_FS(void* base);
		
		virtual Init_RD::DirectoryNode* root() const noexcept override
		{
			return _root;
		}
		
		virtual ReadOnlyFactory& factory() noexcept override
		{
			return _factory;
		}
	};
	
	
}
}
#endif