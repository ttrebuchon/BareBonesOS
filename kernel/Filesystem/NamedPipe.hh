#ifndef INCLUDED_FILESYSTEM_NAMEDPIPE_HH
#define INCLUDED_FILESYSTEM_NAMEDPIPE_HH


#include "Node.hh"

namespace Kernel
{

class Pipe;

namespace FS
{
	
	
	class PipeNode : public virtual Node
	{
		public:
		typedef Pipe pipe_type;
		typedef pipe_type* pipe_ptr;
		typedef const pipe_type* const_pipe_ptr;
		
		protected:
		pipe_ptr _pipe;
		
		
		
		public:
		
		
		virtual uint64_t read(uint64_t start, uint64_t len, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override {};
		virtual void close() override {};
		
		pipe_ptr pipe() const noexcept
		{
			return _pipe;
		}
		
		
		virtual const PipeNode* as_pipe() const noexcept
		{ return this; }
		virtual PipeNode* as_pipe() noexcept
		{ return this; }
	};
	
}
}

#endif