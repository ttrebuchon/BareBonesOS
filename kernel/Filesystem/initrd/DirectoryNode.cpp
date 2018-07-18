#include "DirectoryNode.hh"

namespace Kernel { namespace FS { namespace Init_RD {


    DirectoryNode::DirectoryNode(const char* name) : FS::DirectoryNode(NodeType::Directory)
    {
        this->_name = name;
    }




    uint64_t DirectoryNode::read(uint64_t, uint64_t, uint8_t*)
    {
        // TODO
		ASSERT(false);
    }


    uint64_t DirectoryNode::write(uint64_t, uint64_t, const uint8_t*)
    {
        // TODO
		ASSERT(false);
    }


    void DirectoryNode::open()
    {
        // TODO
		ASSERT(false);
    }


    void DirectoryNode::close()
    {
        // TODO
		ASSERT(false);
    }


    DirEnt* DirectoryNode::readDir(uint32_t)
    {
        // TODO
		ASSERT(false);
    }


    Node* DirectoryNode::findDir(const char* name)
    {
        // TODO
		ASSERT(false);
    }


    void DirectoryNode::addChild(Node* child)
    {
        children.push_back(child);
    }



}
}
}