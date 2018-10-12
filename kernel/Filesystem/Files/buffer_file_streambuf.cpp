#include "buffer_file_streambuf.hh"
#include "../FileNode.hh"


namespace Kernel::FS
{
	buffer_file_streambuf::buffer_file_streambuf(FileNode_v* n, size_t buffer_len) : File_streambuf(n), buffer_len(buffer_len), rstart(0), rend(0), rdata(nullptr), wstart(0), wend(0), wdata(nullptr), write_requested(false)
	{
		
	}
	
	buffer_file_streambuf::buffer_file_streambuf(FileNode_v* n) : buffer_file_streambuf(n, default_buffer_len)
	{
		
	}
	
	buffer_file_streambuf::~buffer_file_streambuf()
	{
		if (wdata && write_requested)
		{
			this->buffer_file_streambuf::fileSync();
		}
		
		if (wdata)
		{
			delete[] wdata;
		}
		
		if (rdata)
		{
			delete[] rdata;
		}
		
		rstart = rend = 0;
		wstart = wend = 0;
	}
	
	unsigned char* buffer_file_streambuf::addrForPos(size_t pos, bool forReading, size_t* remainingContinuous, unsigned char** seqBegin)
	{
		if (forReading)
		{
			if (pos >= rstart && pos < rend)
			{
				if (remainingContinuous)
				{
					*remainingContinuous = (rend - pos);
				}
				
				if (seqBegin)
				{
					*seqBegin = rdata;
				}
				
				return rdata + (pos - rstart);
			}
			
			if (rdata != nullptr)
			{
				if (rdata != wdata)
				{
					delete[] rdata;
					rdata = nullptr;
				}
				rstart = rend = 0;
			}
			
			if (pos >= wstart && pos < wend)
			{
				rdata = wdata;
				rstart = wstart;
				rend = wend;
			}
			else
			{
				size_t len = buffer_len;
				if (__node->size() <= pos)
				{
					return nullptr;
				}
				else if (__node->size() - pos < buffer_len)
				{
					len = __node->size() - pos;
				}
				
				rdata = new unsigned char[len];
				rstart = pos;
				rend = rstart + len;
				
				__node->read(pos, len, rdata);
			}
			
			if (remainingContinuous)
			{
				*remainingContinuous = (rend - pos);
			}
			
			if (seqBegin)
			{
				*seqBegin = rdata;
			}
			
			return rdata + (pos - rstart);
		}
		else
		{
			if (!write_requested)
			{
				if (wdata)
				{
					write_requested = true;
					fileSync();
					if (rstart <= wstart && rend >= wstart)
					{
						rdata[wstart - rstart] = wdata[0];
					}
					delete[] wdata;
					wstart = wend = 0;
				}
				else
				{
					wstart = pos;
					wend = pos+1;
					wdata = new unsigned char[1];
					if (remainingContinuous)
					{
						*remainingContinuous = 1;
					}
					if (seqBegin)
					{
						*seqBegin = wdata;
					}
					return wdata;
				}
			}
			
			if (pos >= wstart && pos < wend)
			{
				if (remainingContinuous)
				{
					*remainingContinuous = (wend - pos);
				}
				
				if (seqBegin)
				{
					*seqBegin = wdata;
				}
				
				return wdata + (pos - wstart);
			}
			
			if (wdata != nullptr)
			{
				fileSync();
				if (wdata != rdata)
				{
					delete[] wdata;
					wdata = nullptr;
				}
				wstart = wend = 0;
			}
			
			if (pos >= rstart && pos < rend)
			{
				wdata = rdata;
				wstart = rstart;
				wend = rend;
			}
			else
			{
				size_t len = buffer_len;
				if (__node->size() <= pos)
				{
					len = 0;
				}
				else if (__node->size() - pos < buffer_len)
				{
					len = __node->size() - pos;
				}
				
				wdata = new unsigned char[buffer_len];
				memset(wdata, 0, buffer_len);
				wstart = pos;
				wend = wstart + buffer_len;
				
				__node->read(pos, len, wdata);
			}
			
			if (remainingContinuous)
			{
				*remainingContinuous = (wend - pos);
			}
			
			if (seqBegin)
			{
				*seqBegin = wdata;
			}
			
			return wdata + (pos - wstart);
			
			// TODO
			assert(NOT_IMPLEMENTED);
		}
	}
	
	size_t buffer_file_streambuf::basePosForPos(const size_t pos) const noexcept
	{
		if (pos >= rstart && pos < rend)
		{
			return rstart;
		}
		else if (pos >= wstart && pos < wend)
		{
			return wstart;
		}
		else
		{
			return pos;
		}
	}
	
	int buffer_file_streambuf::fileSync()
	{
		if (wdata)
		{
			/*assert(pptr());
			assert(pptr() >= (char*)wdata);
			assert(pptr() <= (char*)wdata + (wend - wstart));*/
			return __node->write(wstart, (wstart - wend), wdata);
		}
		else
		{
			return 0;
		}
		// TODO
		assert(NOT_IMPLEMENTED);
	}
	
}