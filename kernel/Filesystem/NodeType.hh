#ifndef INCLUDED_NODETYPE_HH
#define INCLUDED_NODETYPE_HH

namespace Kernel { namespace Filesystem {
	
	enum class NodeType
	{
		Unknown = 0,
		Directory = 1,
		File = 2,
		Link = 4,
	};
	
}
}

#endif