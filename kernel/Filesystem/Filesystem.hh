#ifndef INCLUDED_FILESYSTEM_HH
#define INCLUDED_FILESYSTEM_HH

#include <Common.h>
#include <Utils/string>

namespace Kernel { namespace Filesystem {
	
	class Node;
	
	class Filesystem
	{
		private:
		
		protected:
		Node* root;
		
		public:
		static Filesystem* Current;
		
		Node* getNode(const Utils::string& path, const Utils::string& relative = Utils::string());
	};
	
}
}

#endif