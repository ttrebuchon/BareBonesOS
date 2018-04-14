#include "File.hh"
#include "Files/File_streambuf.hpp"

namespace Kernel { namespace Filesystem
{
	
	File::File(FileNode* fnode) : __node(fnode), sbuf(File_streambuf::GetForNode(fnode)), in(sbuf), out(sbuf)
	{
		
	}
	
	File::~File()
	{
		sbuf->pubsync();
		in.rdbuf(nullptr);
		out.rdbuf(nullptr);
		delete sbuf;
	}
	
}
}