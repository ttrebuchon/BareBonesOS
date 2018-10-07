#ifndef INCLUDED_KERNEL_SCHEDULING_FILESYSTEM_CONTEXT_H
#define INCLUDED_KERNEL_SCHEDULING_FILESYSTEM_CONTEXT_H

#include <Common.h>

START_NS(Kernel)
#ifdef __cplusplus
namespace FS
{
	class DirectoryNode_v;
	class Filesystem;
}
#endif
C_CODE




typedef struct filesystem_context
{
	IF_CPP_ELSE(FS::DirectoryNode_v*, void*) cwd;
	IF_CPP_ELSE(FS::Filesystem*, void*) fs;
}
filesystem_ctx_t;


C_END
END_NS // Kernel

#endif