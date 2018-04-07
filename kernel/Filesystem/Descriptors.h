#ifndef INCLUDED_DESCRIPTORS_H
#define INCLUDED_DESCRIPTORS_H

#include <Common.h>
#include "OpenFileHandle.h"

#ifdef __cplusplus
#include <Utils/vector>
#include <kernel/ResourceHandles/ResourceHandle.hh>
#include <kernel/ResourceHandles/FileHandle.hh>

namespace Kernel { namespace Filesystem {
	
	class FileNode;
	
	C_CODE
	struct OpenFile_Hndl;
	struct FileDescriptors final
	{
		private:
		bool needNext;
		int nextFree;
		Utils::vector<ResourcePtr<FileHandle>> descriptors;
		
		void reset() noexcept;
		
		// In case we ever do decide to
		// have a descriptor set take
		// an action when made in/active,
		// we have these functions to
		// expand
		void makeInactive() noexcept;
		void makeActive() noexcept;
		
		
		public:
		FileDescriptors();
		
		int map(ResourcePtr<FileHandle>&&);
		bool unmap(int);
		File* resolve(int fd);
		
		
		
		static FileDescriptors* Current;
		friend class FileDescriptors_Hndl;
	};
	C_END
	
	class FileDescriptors_Hndl : public ResourceHandle
	{
		protected:
		FileDescriptors* desc;
		
		virtual void __cleanup() noexcept;
		virtual void __makeActive() noexcept;
		virtual void __makeInactive() noexcept;
		
		public:
		
		virtual ~FileDescriptors_Hndl() noexcept;
		
	};

	
}
}

#endif

C_CODE
C_namespace(Kernel)
C_namespace(Filesystem)



struct FileDescriptors* get_file_descriptors();
void init_kernel_file_descriptors();
struct OpenFile_Hndl resolve_file_descriptor_for(struct FileDescriptors*, int fd);
struct OpenFile_Hndl resolve_file_descriptor(int fd);

C_namespace_e(Filesystem)
C_namespace_e(Kernel)
C_END

#endif