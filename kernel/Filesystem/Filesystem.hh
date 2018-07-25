#ifndef INCLUDED_FILESYSTEM_HH
#define INCLUDED_FILESYSTEM_HH

#include <Common.h>
#include <Utils/string>

namespace Drivers
{
	class Disk;
}

namespace Kernel { namespace FS {
	
	class Node;
	class Factory;
	
	class Filesystem
	{
		private:
		
		protected:
		Node* _root;
		
		public:
		static Filesystem* Current;
		
		virtual Node* getNode(const Utils::string& path, const Utils::string& relative = Utils::string());
		
		virtual Node* root() const noexcept
		{
			return _root;
		}
		
		virtual Factory& factory() noexcept = 0;
	};
	
}
}

#endif