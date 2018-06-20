#ifndef INCLUDED_NODETYPE_HH
#define INCLUDED_NODETYPE_HH

#include <Utils/EnumOperators.hh>

namespace Kernel { namespace FS {
	
	enum class NodeType : short
	{
		Unknown = 0,
		Directory = 1,
		File = 2,
		Link = 4,
		RAMFile = 8,
	};
	
	ENUM_OPS(NodeType, short);
	
}
}

#endif