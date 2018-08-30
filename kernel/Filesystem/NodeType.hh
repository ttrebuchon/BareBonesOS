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
		Device = 16,
		Block = 32 | Device,
		Char = 64 | Device,
		Pipe = 128,
		Other= 256,
		FSSpecific = 256,
	};
	
	ENUM_OPS(NodeType, short);
	
}
}

#endif