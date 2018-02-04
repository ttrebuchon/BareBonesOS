#include "initrd.hh"

namespace Kernel { namespace Filesystem {
	
	Node* readRecord(void* addr, void**);
	Node* readFolder(void*, void**);
	Node* readFile(void*, void**);
	
	Node* init_initrd(void* addr)
	{
		return readRecord(addr, nullptr);
	}
	
	Node* readRecord(void* addr, void** end)
	{
		int folder = *(int*)addr;
		
		if (folder == 1)
		{
			return readFolder(((int*)addr + 1), end);
		}
		else
		{
			return readFile(((int*)addr + 1), end);
		}
	}
	
	Node* readFile(void* addr, void** endAddr)
	{
		unsigned long span = *(unsigned long*)addr;
		char* recordName = (char*)(addr + sizeof(unsigned long));
		int i = 0;
		while (recordName[i++] != 0x0) ;
		void* data = recordName + i;
		auto size = span - sizeof(int) - (data - addr);
		if (endAddr != nullptr)
		{
			*endAddr = data + size;
		}
		
	}
	
	Node* readFolder(void* addr, void** end)
	{
		unsigned long span = *(unsigned long*)addr;
		char* recordName = (char*)(addr + sizeof(unsigned long));
		int i = 0;
		while (recordName[i++] != 0x0) ;
		void* data = recordName + i;
		unsigned int count = *(unsigned int*)data;
		void* entries = data + sizeof(unsigned int);
		
		void* ptr = entries;
		void* tmp;
		for (int j = 0; j < count; ++j)
		{
			Node* entry = readRecord(ptr, &tmp);
			ptr = tmp;
		}
		if (end != nullptr)
		{
			*end = ptr;
		}
	}
}
}