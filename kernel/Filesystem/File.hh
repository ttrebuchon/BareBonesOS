#ifndef INCLUDED_FILE_HH
#define INCLUDED_FILE_HH

#include <Common.h>
#include <Utils/vector>
//#include "Files/iofstream.hh"
#include <Utils/iostream>
#include "Files/File_streambuf.hh"

namespace Kernel { namespace FS
{
	class FileNode;
	class FileRep;
	
	class File
	{
		protected:
		FileNode* __node;
		File_streambuf* sbuf;
		FileRep* _rep;
		
		
		
		public:
		Utils::basic_istream<char> in;
		Utils::basic_ostream<char> out;
		
		File(FileNode*);
		~File();
	};
	
	
	
	
}
}

#endif