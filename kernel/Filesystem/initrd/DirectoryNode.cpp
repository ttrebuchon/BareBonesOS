#include "DirectoryNode.hh"

namespace Kernel { namespace Filesystem { namespace Init_RD {


    DirectoryNode::DirectoryNode(const char* name) : Filesystem::DirectoryNode(NodeType::Directory)
    {
        this->_name = name;
    }




    uint32_t DirectoryNode::read(uint32_t, uint32_t, uint8_t*)
    {
        // TODO
		assert(false);
    }


    uint32_t DirectoryNode::write(uint32_t, uint32_t, const uint8_t*)
    {
        // TODO
		assert(false);
    }


    void DirectoryNode::open()
    {
        // TODO
		assert(false);
    }


    void DirectoryNode::close()
    {
        // TODO
		assert(false);
    }


    DirEnt* DirectoryNode::readDir(uint32_t)
    {
        // TODO
		assert(false);
    }


    Node* DirectoryNode::findDir(const char* name)
    {
        // TODO
		assert(false);
    }


    void DirectoryNode::addChild(Node* child)
    {
        children.push_back(child);
    }



}
}
}