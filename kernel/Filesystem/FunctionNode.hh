#ifndef INCLUDED_FILESYSTEM_FUNCTIONNODE_HH
#define INCLUDED_FILESYSTEM_FUNCTIONNODE_HH

#include <Common.h>
#include "Node.hh"
#include <Utils/shared_ptr>

namespace Kernel { namespace FS {
	
	class FunctionNode : public virtual Node
	{
		protected:
		
		public:
		FunctionNode(const NodeType = NodeType::Function);
		
		virtual const char* function() const noexcept = 0;
		
		virtual const FunctionNode* as_function() const noexcept override
		{ return this; }
		virtual FunctionNode* as_function() noexcept override
		{ return this; }
    };
}
}
#endif