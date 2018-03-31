

#include "sys/stat.h"
#include <kernel/Filesystem/Filesystem.hh>



#undef lstat
#undef __lstat
extern "C" {
int
attribute_hidden
__lstat (const char* path, struct stat *buf)
{
	if (Kernel::Filesystem::Filesystem::Current)
	{
		auto n = Kernel::Filesystem::Filesystem::Current->getNode(path);
		if (n)
		{
			//TODO
		}
	}
  return -1;
}

weak_hidden_alias (__lstat, lstat)

}