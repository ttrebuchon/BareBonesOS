#ifndef INCLUDED_FILESYSTEM_INITRD_HH
#define INCLUDED_FILESYSTEM_INITRD_HH

#include <Common.h>

namespace Kernel { namespace Filesystem {
	
	class Node;
	
	Node* init_initrd(void* addr);
}
}
#endif