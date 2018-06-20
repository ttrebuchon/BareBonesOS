#ifndef INCLUDED_DIRENT_HH
#define INCLUDED_DIRENT_HH

#include <Common.h>

namespace Kernel { namespace FS {

    class DirEnt
    {
        public:
        char name[128];
        uint32_t ino;
    };

}
}
#endif