#ifndef INCLUDED_KERNEL_FILESYSTEM_FILES_BUFFER_FILE_STREAMBUF_HH
#define INCLUDED_KERNEL_FILESYSTEM_FILES_BUFFER_FILE_STREAMBUF_HH

#include <Common.h>
#include "File_streambuf.hh"


namespace Kernel::FS
{
	class buffer_file_streambuf : public File_streambuf
	{
		public:
		constexpr static size_t default_buffer_len = 4096;
		
		protected:
		size_t buffer_len;
		
		size_t rstart, rend;
		unsigned char* rdata;
		
		size_t wstart, wend;
		unsigned char* wdata;
		bool write_requested;
		
		
		virtual unsigned char* addrForPos(size_t, bool forReading, size_t* remainingContinuous = nullptr, unsigned char** seqBegin = nullptr) override;
		virtual size_t basePosForPos(const size_t pos) const noexcept override;
		virtual int fileSync() override;
		
		public:
		
		buffer_file_streambuf(FileNode_v*, size_t buffer_len);
		buffer_file_streambuf(FileNode_v*);
		virtual ~buffer_file_streambuf();
		
	};
	
}


#endif