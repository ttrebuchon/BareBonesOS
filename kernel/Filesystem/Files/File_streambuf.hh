#ifndef INCLUDED_FILE_STREAMBUF_HH
#define INCLUDED_FILE_STREAMBUF_HH

#include <Common.h>
#include <Utils/streambuf>

namespace Kernel { namespace Filesystem
{
	class FileNode;
	class File;
	
	
	
	class File_streambuf : public Utils::basic_streambuf<char>
	{
		private:
		typedef Utils::basic_streambuf<char> Base;
		
		protected:
		typedef Utils::ios_base ios_base;
		
		public:
		typedef typename Base::char_type char_type;
		typedef typename Base::int_type int_type;
		typedef typename Base::pos_type pos_type;
		typedef typename Base::off_type off_type;
		typedef typename Base::traits_type traits_type;
		
		
		protected:
		FileNode* __node;
		size_t eback_pos;
		size_t pbase_pos;
		size_t size;
		
		
		virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in|ios_base::out);
		virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in|ios_base::out);
		virtual Utils::streamsize showmanyc();
		virtual Utils::streamsize xsgetn(char_type* s, Utils::streamsize n);
		virtual int_type underflow();
		virtual int_type uflow();
		
		virtual int_type pbackfail(int_type c = traits_type::eof());
		
		virtual Utils::streamsize xsputn(const char_type* s, Utils::streamsize n);
		virtual int_type overflow(int_type c = traits_type::eof());
		virtual int sync();
		virtual File_streambuf* setbuf(char_type*, Utils::streamsize);
		
		
		
		
		virtual unsigned char* addrForPos(size_t, bool forReading, size_t* remainingContinuous = nullptr, unsigned char** seqBegin = nullptr) = 0;
		virtual size_t basePosForPos(const size_t pos) const noexcept = 0;
		
		
		
		public:
		
		File_streambuf(FileNode*);
		
		static File_streambuf* GetForNode(FileNode*);
	};
	
	
}
}

#endif