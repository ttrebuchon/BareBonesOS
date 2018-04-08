#include "File_streambuf.hh"
#include "BlockFile.hh"
#include "../FileNode.hh"

namespace Kernel { namespace Filesystem
{
	
	File_streambuf::File_streambuf(FileNode* fnode) : Utils::basic_streambuf<char>(), __node(fnode), eback_pos(0), pbase_pos(0), size(0)
	{
		ASSERT(fnode != nullptr);
		size = __node->size();
	}
	
	
	
	
	auto File_streambuf::seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which) -> pos_type
	{
		size_t pos;
		switch (way)
		{
			case ios_base::beg:
			{
				pos = off;
			}
			break;
			
			case ios_base::cur:
			{
				if (which & ios_base::out)
				{
					pos = off + pbase_pos + (pptr() - pbase());
				}
				else if (which & ios_base::in)
				{
					pos = off + eback_pos + (gptr() - eback());
				}
				else
				{
					ASSERT(false);
				}
				
			}
			break;
			
			case ios_base::end:
			{
				pos = size - off;
			}
			break;
			
			default:
			ASSERT(false);
		}
		
		
		unsigned char* seqStart;
		size_t rem;
		auto addr = addrForPos(pos, (which & ios_base::in != 0), &rem, &seqStart);
		ASSERT(addr != nullptr);
		if (which & ios_base::out)
		{
			setp(reinterpret_cast<char*>(seqStart), reinterpret_cast<char*>(addr + rem));
			pbump(addr - seqStart);
			pbase_pos = basePosForPos(pos);
		}
		else
		{
			setg(reinterpret_cast<char*>(seqStart), reinterpret_cast<char*>(addr), reinterpret_cast<char*>(addr + rem));
			eback_pos = basePosForPos(pos);
		}
		
		 
		 // TODO (Return value)
		 ASSERT(false);
	}
	
	Utils::streamsize File_streambuf::showmanyc()
	{
		// Working as intended
		return 0;
	}
	
	Utils::streamsize File_streambuf::xsgetn(char_type* s, Utils::streamsize n)
	{
		
	}
	
	
	
	
	
	
	
	
	File_streambuf* File_streambuf::GetForNode(FileNode* fnode)
	{
		// TODO
		return new BlockFile(fnode);
	}
	
}
}