#ifndef INCLUDED_FILESYSTEM_HH
#define INCLUDED_FILESYSTEM_HH

#include <Common.h>
#include <Utils/string>
#include "Path_Fwd.hh"

namespace Drivers
{
	class Disk;
}

namespace Kernel { namespace FS {
	
	class Node;
	class Factory;
	class DirectoryNode_v;
	
	
	class Filesystem
	{
		private:
		
		protected:
		Node* _root;
		
		public:
		static Filesystem* Current;
		
		virtual Node* getNode(const Utils::string& path, const Utils::string& relative = Utils::string());
		virtual Node* getNode(const Path_t& path);
		
		
		virtual Node* root() const noexcept __attribute__((__always_inline__))
		{
			return _root;
		}
		
		virtual DirectoryNode_v* rootd() const noexcept;
		
		virtual Factory& factory() noexcept = 0;
	};
	
}
}

#endif