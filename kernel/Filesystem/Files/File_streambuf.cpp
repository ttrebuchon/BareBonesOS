#include "File_streambuf.hh"
#include "BlockFile.hh"
#include "../FileNode.hh"
#include "buffer_file_streambuf.hh"

namespace Kernel { namespace FS
{
	
	File_streambuf::File_streambuf(FileNode_v* fnode) : Utils::basic_streambuf<char>(), __node(fnode), eback_pos(0), pbase_pos(0), size(0)
	{
		assert(__node);
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
					pos = outPos(off);
					//pos = off + pbase_pos + (pptr() - pbase());
				}
				else if (which & ios_base::in)
				{
					pos = inPos(off);
					//pos = off + eback_pos + (gptr() - eback());
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
		auto addr = addrForPos(pos, ((which & ios_base::in) != 0), &rem, &seqStart);
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
		
		 return pos;
		 // TODO (Return value)
		 ASSERT(false);
	}

	auto File_streambuf::seekpos(pos_type sp, ios_base::openmode which) -> pos_type
	{
		// TODO
		ASSERT(false);
	}
	
	Utils::streamsize File_streambuf::showmanyc()
	{
		// Working as intended
		return 0;
	}
	
	Utils::streamsize File_streambuf::xsgetn(char_type* s, Utils::streamsize n)
	{
		// TODO: Account for errors,
		// EOF, etc.
		auto ptr = gptr();
		auto back = egptr();
		
		Utils::streamsize i = 0;
		while (i < n)
		{
			if (ptr == back)
			{
				seekoff(back - gptr(), ios_base::cur, ios_base::in);
				ptr = gptr();
				back = egptr();
				ASSERT(ptr != back);
			}
			s[i++] = *(ptr++);
		}
		
		gbump(ptr - gptr());
		
		return i;
	}
	
	auto File_streambuf::underflow() -> int_type
	{
		ASSERT(gptr() == egptr());
		
		unsigned char* seqBegin;
		size_t rem;
		auto addr = addrForPos(inPos(), true, &rem, &seqBegin);
		eback_pos = basePosForPos(inPos());
		//ASSERT(addr);
		if (!addr)
		{
			return traits_type::eof();
		}
		
		setg(reinterpret_cast<char_type*>(seqBegin), reinterpret_cast<char_type*>(addr), reinterpret_cast<char_type*>(addr + rem));
		return traits_type::to_int_type(*gptr());
	}
	
	/*auto File_streambuf::pbackfail(int_type c) -> int_type
	{
		
	}*/
	
	Utils::streamsize File_streambuf::xsputn(const char_type* s, Utils::streamsize n)
	{
		// TODO: Account for errors,
		// EOF, etc.
		auto ptr = pptr();
		auto back = epptr();
		
		Utils::streamsize i = 0;
		while (i < n)
		{
			if (ptr == back)
			{
				seekoff(back - pptr(), ios_base::cur, ios_base::in);
				ptr = pptr();
				back = epptr();
				ASSERT(ptr != back);
			}
			*(ptr++) = s[i++];
		}
		
		pbump(ptr - pptr());
		
		return i;
	}
	
	auto File_streambuf::overflow(int_type c) -> int_type
	{
		ASSERT(pptr() == epptr());
		
		unsigned char* seqBegin;
		size_t rem;
		auto addr = addrForPos(outPos(), false, &rem, &seqBegin);
		pbase_pos = basePosForPos(outPos());
		ASSERT(addr);
		if (!addr)
		{
			return traits_type::eof();
		}
		
		setp(reinterpret_cast<char_type*>(seqBegin), reinterpret_cast<char_type*>(addr + rem));
		pbump(addr - seqBegin);
		return traits_type::to_int_type(*pptr());
	}
	
	int File_streambuf::sync()
	{
		return fileSync();
	}
	
	
	
	
	
	size_t File_streambuf::inPos(off_t off) const noexcept
	{
		return eback_pos + off + (gptr() - eback());
	}
	
	size_t File_streambuf::outPos(off_t off) const noexcept
	{
		return pbase_pos + off + (pptr() - pbase());
	}
	
	
	
	
	
	
	File_streambuf* File_streambuf::GetForNode(FileNode_v* fnode)
	{
		return new buffer_file_streambuf(fnode);
		// TODO
		return new BlockFile(fnode);
	}
	
}
}