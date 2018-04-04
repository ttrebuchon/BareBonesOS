#include "Descriptors.h"
#include <Utils/map>

#define DEFAULT_DESC 16


namespace Kernel { namespace Filesystem {
	
	FileDescriptors* FileDescriptors::Current = nullptr;
	
	FileDescriptors::FileDescriptors() : needNext(false), nextFree(0), descriptors()
	{
		
	}
	
	int FileDescriptors::map(FileNode* n)
	{
		int index;
		if (needNext)
		{
			index = -1;
			int nIndex = -1;
			int i = -1;
			int offset = nextFree;
			if (nextFree < 0)
			{
				offset = 0;
			}
			for (auto it = descriptors.begin() + offset, e = descriptors.end(); it != e; ++it, ++i)
			{
				if (*it == nullptr)
				{
					if (index == -1)
					{
						index = i;
					}
					else
					{
						nIndex = i;
						break;
					}
				}
			}
			
			if (nIndex != -1)
			{
				nextFree = nIndex;
				descriptors[index] = n;
				needNext = false;
				return index;
			}
			else
			{
				auto oldSize = descriptors.size();
				descriptors.resize(1.5*oldSize + 2);
				
				needNext = false;
				
				if (index != -1)
				{
					descriptors[index] = n;
					nextFree = oldSize;
					return index;
				}
				else
				{
					descriptors[oldSize] = n;
					nextFree = oldSize + 1;
					return oldSize;
				}
			}
		}
		
		
		needNext = true;
		index = nextFree++;
		descriptors[index] = n;
		if (nextFree >= descriptors.size())
		{
			nextFree = -1;
		}
		return index;
	}
	
	
	FileNode* FileDescriptors::resolve(int fd)
	{
		if (fd > 0 && fd < descriptors.size())
		{
			return descriptors[fd];
		}
		
		// TODO: Better error
		return nullptr;
	}
	
	
	extern "C" {
	
	void init_kernel_file_descriptors()
	{
		if (!FileDescriptors::Current)
		{
			FileDescriptors::Current = new FileDescriptors;
		}
	}
	
	FileDescriptors* get_file_descriptors()
	{
		return FileDescriptors::Current;
	}
	
	OpenFile_Hndl resolve_file_descriptor(struct FileDescriptors* desc, int fd)
	{
		if (desc)
		{
			auto node = desc->resolve(fd);
			if (node)
			{
				return OpenFile_Hndl{node};
			}
		}
		
		return OpenFile_Hndl{nullptr};
	}
	
	
	}
}
}

