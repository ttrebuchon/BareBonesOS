#include "FileNode.hh"

namespace Kernel { namespace Filesystem { namespace Init_RD {

    FileNode::FileNode(const char* name, void* data, uint32_t size)
    {
        this->name = name;
    }

    uint32_t FileNode::read(uint32_t, uint32_t, uint8_t*)
    {

    }


    uint32_t FileNode::write(uint32_t, uint32_t, uint8_t*)
    {

    }


    void FileNode::open()
    {

    }


    void FileNode::close()
    {

    }


    DirEnt* FileNode::readDir(uint32_t)
    {

    }


    Node* FileNode::findDir(const char* name)
    {

    }




}
}
}