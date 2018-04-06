#ifndef INCLUDED_OPENFILEHANDLE_H
#define INCLUDED_OPENFILEHANDLE_H

#include <Common.h>




C_CODE
C_namespace(Kernel)
C_namespace(Filesystem)

IF_CPP(class FileNode);


struct OpenFile_Hndl
{
	IF_CPP_ELSE(FileNode*, void*) ptr;
};




C_namespace_e(Filesystem)
C_namespace_e(Kernel)
C_END


#endif