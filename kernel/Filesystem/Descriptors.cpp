#include "Descriptors.h"
#include <Utils/map>

#define DEFAULT_DESC 16


namespace Kernel { namespace Filesystem {
	
	FileDescriptors* FileDescriptors::Current = nullptr;
	
	FileDescriptors::FileDescriptors() : handles(), descriptors()
	{
		
	}
	
	void FileDescriptors::reset() noexcept
	{
		handles.clear();
		for (auto len = descriptors.size(); len > 0; --len)
		{
			descriptors.pop();
		}
		
		// If clearing doesn't reallocate,
		// force it to shrink to <= 5
		if (handles.capacity() > 5)
		{
			handles.resize(5);
			handles.shrink_to_fit();
			handles.clear();
		}
		
		for (int i = 0, c = handles.size(); i < c; ++i)
		{
			descriptors.push(i);
		}
	}
	
	void FileDescriptors::expand() noexcept
	{
		auto oldSize = handles.size();
		auto newSize = oldSize*1.5 + 2;
		
		handles.resize(newSize);
		for (auto i = oldSize; i < newSize; ++i)
		{
			descriptors.push(i);
		}
	}
	
	void FileDescriptors::makeInactive() noexcept
	{
		if (this == FileDescriptors::Current)
		{
			FileDescriptors::Current = nullptr;
			for (auto& fd : handles)
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
		assert(this != FileDescriptors::Current);
		FileDescriptors::Current = this;
		for (auto& fd : handles)
		{
			if (fd)
			{
				fd->makeActive();
			}
		}
	}
	
	
	int FileDescriptors::map(ResourcePtr<FileHandle>&& n)
	{
		/*int index;
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
		return index;*/
		
		if (descriptors.empty())
		{
			expand();
			assert(!descriptors.empty());
		}
		auto index = descriptors.top();
		descriptors.pop();
		
		handles[index] = Utils::move(n);
		return index;
	}
	
	bool FileDescriptors::unmap(int fd)
	{
		#ifdef DEBUG
		if (fd > 0)
		{
			assert((size_t)fd < handles.size());
		}
		#endif
		
		if (handles[fd])
		{
			handles[fd] = nullptr;
			descriptors.push(fd);
			return true;
		}
		
		return false;
		
		/*if (fd > 0 && fd < descriptors.size())
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
		
		return false;*/
	}
	
	
	File* FileDescriptors::resolve(int fd)
	{
		#ifdef DEBUG
		if (fd > 0)
		{
			assert((size_t)fd < handles.size());
		}
		#endif
		
		if (handles[fd])
		{
			return handles[fd]->file();
		}
		// TODO: Better error
		return nullptr;
		/*if (fd > 0 && fd < descriptors.size())
		{
			if (descriptors[fd] != nullptr)
			{
				return descriptors[fd]->file();
			}
		}
		
		// TODO: Better error
		return nullptr;*/
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