#ifndef INCLUDED_FILESYSTEM_NODE_HH
#define INCLUDED_FILESYSTEM_NODE_HH

#include <Common.h>

namespace Kernel { namespace Filesystem {

    class DirEnt;

    class Node
    {
        protected:

        public:
        uint32_t inode;

        virtual uint32_t read(uint32_t, uint32_t, uint8_t*) = 0;
        virtual uint32_t write(uint32_t, uint32_t, uint8_t*) = 0;
        virtual void open() = 0;
        virtual void close() = 0;
        virtual DirEnt* readDir(uint32_t) = 0;
        virtual Node* findDir(const char* name) = 0;

    };




}
}
#endif