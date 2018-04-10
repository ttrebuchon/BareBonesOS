#include "Descriptors.h"
#include <Utils/map>

#define DEFAULT_DESC 16


namespace Kernel { namespace Filesystem {
	
	FileDescriptors* FileDescriptors::Current = nullptr;
	
	FileDescriptors::FileDescriptors() : needNext(false), nextFree(0), descriptors()
	{
		
	}
	
	void FileDescriptors::reset() noexcept
	{
		needNext = false;
		nextFree = 0;
		descriptors.clear();
		
		// If clearing doesn't reallocate,
		// force it to shrink to <= 5
		if (descriptors.capacity() > 5)
		{
			descriptors.resize(5);
			descriptors.shrink_to_fit();
			descriptors.clear();
		}
	}
	
	void FileDescriptors::makeInactive() noexcept
	{
		if (this == FileDescriptors::Current)
		{
			FileDescriptors::Current = nullptr;
			for (auto& fd : descriptors)
			{
				if (fd)
				{
					fd->makeInactive();
				}
			}
		}
	}
	
	void FileDescriptors::makeActive() noexcept
	{
		ASSERT(this != FileDescriptors::Current);
		FileDescriptors::Current = this;
		for (auto& fd : descriptors)
		{
			if (fd)
			{
				fd->makeActive();
			}
		}
	}
	
	
	int FileDescriptors::map(ResourcePtr<FileHandle>&& n)
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
				descriptors[index] = Utils::move(n);
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
					descriptors[index] = Utils::move(n);
					nextFree = oldSize;
					return index;
				}
				else
				{
					descriptors[oldSize] = Utils::move(n);
					nextFree = oldSize + 1;
					return oldSize;
				}
			}
		}
		
		
		needNext = true;
		index = nextFree++;
		descriptors[index] = Utils::move(n);
		if (nextFree >= descriptors.size())
		{
			nextFree = -1;
		}
		return index;
	}
	
	bool FileDescriptors::unmap(int fd)
	{
		
		if (fd > 0 && fd < descriptors.size())
		{
			if (descriptors[fd] != nullptr)
			{
				descriptors[fd] = nullptr;
				if (fd < nextFree || needNext)
				{
					nextFree = fd;
					needNext = false;
				}
				return true;
			}
		}
		
		return false;
	}
	
	
	File* FileDescriptors::resolve(int fd)
	{
		if (fd > 0 && fd < descriptors.size())
		{
			if (descriptors[fd] != nullptr)
			{
				return descriptors[fd]->file();
			}
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
	
	OpenFile_Hndl resolve_file_descriptor_for(struct FileDescriptors* desc, int fd)
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
	
	OpenFile_Hndl resolve_file_descriptor(int fd)
	{
		if (FileDescriptors::Current)
		{
			auto file = FileDescriptors::Current->resolve(fd);
			if (file)
			{
				return OpenFile_Hndl{file};
			}
		}
		
		return OpenFile_Hndl{nullptr};
	}
	
	
	}
	
	
	FileDescriptors_Hndl::~FileDescriptors_Hndl() noexcept
	{
		delete desc;
	}
	
	
	void FileDescriptors_Hndl::__cleanup() noexcept
	{
		desc->reset();
	}
	
	void FileDescriptors_Hndl::__makeActive() noexcept
	{
		if (FileDescriptors::Current)
		{
			FileDescriptors::Current->makeInactive();
		}
		desc->makeActive();
	}

	void FileDescriptors_Hndl::__makeInactive() noexcept
	{
		desc->makeInactive();
	}
	
}
}