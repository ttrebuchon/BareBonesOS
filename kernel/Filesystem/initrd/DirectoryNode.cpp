#include "DirectoryNode.hh"

namespace Kernel { namespace Filesystem { namespace Init_RD {


    DirectoryNode::DirectoryNode(const char* name) : Filesystem::DirectoryNode(NodeType::Directory)
    {
        this->name = name;
    }




    uint32_t DirectoryNode::read(uint32_t, uint32_t, uint8_t*)
    {

    }


    uint32_t DirectoryNode::write(uint32_t, uint32_t, const uint8_t*)
    {

    }


    void DirectoryNode::open()
    {

    }


    void DirectoryNode::close()
    {

    }


    DirEnt* DirectoryNode::readDir(uint32_t)
    {

    }


    Node* DirectoryNode::findDir(const char* name)
    {

    }


    void DirectoryNode::addChild(Node* child)
    {
        children.push_back(child);
    }



}
}
}