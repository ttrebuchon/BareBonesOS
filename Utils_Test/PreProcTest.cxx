#define SPACE(N) ABCDEF##N

#define GEN_AS(N, Y, W) SPACE(1)\
node_ptr<Y> node_ptr<N>::W() const noexcept SPACE(1)\
	{ SPACE(2)\
		typedef Y ntype; SPACE(2)\
		if (_node) SPACE(2)\
		{ SPACE(3)\
			auto other = _node->W(); SPACE(3)\
			if (other) SPACE(3)\
			{ SPACE(4)\
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other)); SPACE(3)\
			} SPACE(2)\
		} SPACE(2)\
		return node_ptr<ntype>(); SPACE(1)\
	} SPACE(1)\
	SPACE(1)\
	
	#define GEN_ALL_AS(Node) SPACE(1)\
	GEN_AS(Node, FileNode_v, as_file) SPACE(1)\
	GEN_AS(Node, DirectoryNode_v, as_directory) SPACE(1)\
	GEN_AS(Node, BlockDeviceNode, as_block_device) SPACE(1)\
	GEN_AS(Node, CharDeviceNode, as_char_device) SPACE(1)\
	GEN_AS(Node, LinkNode, as_link) SPACE(1)\
	GEN_AS(Node, PipeNode, as_pipe) SPACE(1)\
	GEN_AS(Node, FunctionNode, as_function) SPACE(1)\
	
	
	GEN_AS(Node, FileNode_v, as_file)
	
	GEN_AS(Node, DirectoryNode_v, as_directory)
	
	GEN_AS(Node, BlockDeviceNode, as_block_device)
	
	GEN_AS(Node, CharDeviceNode, as_char_device)
	
	GEN_AS(Node, LinkNode, as_link)
	
	GEN_AS(Node, PipeNode, as_pipe)
	
	GEN_AS(Node, FunctionNode, as_function)
	
	
	
	
	GEN_AS(const Node, const FileNode_v, as_file)
	
	GEN_AS(const Node, const DirectoryNode_v, as_directory)
	
	GEN_AS(const Node, const BlockDeviceNode, as_block_device)
	
	GEN_AS(const Node, const CharDeviceNode, as_char_device)
	
	GEN_AS(const Node, const LinkNode, as_link)
	
	GEN_AS(const Node, const PipeNode, as_pipe)
	
	GEN_AS(const Node, const FunctionNode, as_function)
	
	
