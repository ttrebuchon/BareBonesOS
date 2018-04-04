#ifndef INCLUDED_DESCRIPTORS_H
#define INCLUDED_DESCRIPTORS_H

#include <Common.h>
#include "OpenFileHandle.h"

#ifdef __cplusplus
#include <Utils/vector>

namespace Kernel { namespace Filesystem {
	
	class FileNode;
	
	C_CODE
	struct OpenFile_Hndl;
	struct FileDescriptors final
	{
		private:
		bool needNext;
		int nextFree;
		Utils::vector<FileNode*> descriptors;
		
		
		public:
		FileDescriptors();
		
		int map(FileNode*);
		void unmap(int);
		FileNode* resolve(int fd);
		
		
		
		static FileDescriptors* Current;
	};
	C_END

	
}
}

#endif

C_CODE
C_namespace(Kernel)
C_namespace(Filesystem)



struct FileDescriptors* get_file_descriptors();
void init_kernel_file_descriptors();
struct OpenFile_Hndl resolve_file_descriptor(struct FileDescriptors*, int fd);

C_namespace_e(Filesystem)
C_namespace_e(Kernel)
C_END

#endif