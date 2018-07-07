#include "initrd.hh"
#include "initrd/DirectoryNode.hh"
#include "initrd/FileNode.hh"

namespace Kernel { namespace FS {
	
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
		char* recordName = (char*)addr + sizeof(unsigned long);
		int i = 0;
		while (recordName[i++] != 0x0) ;
		void* data = recordName + i;
		auto size = span - sizeof(int) - ((addr_t)data - (addr_t)addr);
		if (endAddr != nullptr)
		{
			*endAddr = (char*)data + size;
		}

		auto file = new Init_RD::FileNode(recordName, data, size);
		
		return file;
	}
	
	Node* readFolder(void* addr, void** end)
	{
		unsigned long span = *(unsigned long*)addr;
		char* recordName = (char*)addr + sizeof(unsigned long);
		int i = 0;
		while (recordName[i++] != 0x0) ;
		void* data = recordName + i;
		unsigned int count = *(unsigned int*)data;
		void* entries = (char*)data + sizeof(unsigned int);

		auto dir = new Init_RD::DirectoryNode(recordName);
		
		void* ptr = entries;
		void* tmp;
		for (unsigned int j = 0; j < count; ++j)
		{
			Node* entry = readRecord(ptr, &tmp);
			dir->addChild(entry);
			ptr = tmp;
		}
		if (end != nullptr)
		{
			*end = ptr;
		}
		return dir;
	}
	
	
	
	
	
	InitRD_FS::InitRD_FS(void* base) : Filesystem::Filesystem(), _root(nullptr), _base(base)
	{
		assert(base);
		auto r = init_initrd(base);
		assert(r);
		assert(r->isKind(NodeType::Directory));
		_root = (Init_RD::DirectoryNode*)r;
	}
}
}