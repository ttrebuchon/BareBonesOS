#include "initrd.hh"
#include "initrd/DirectoryNode.hh"
#include "initrd/FileNode.hh"
#include "initrd/initrd_types.hh"
#include "initrd/FunctionNode.hh"
#include "initrd/Factory.hh"

namespace Kernel { namespace FS {
	
	
	
	Node* readRecord(InitRD_FS*, void* addr, void**);
	Node* readFolder(InitRD_FS*, void*, void**);
	Node* readFile(InitRD_FS*, void*, void**);
	Node* readSpecial(InitRD_FS*, void*, void**);
	
	Node* init_initrd(InitRD_FS* fs, void* addr)
	{
		return readRecord(fs, addr, nullptr);
	}
	
	Node* readRecord(InitRD_FS* fs, void* addr, void** end)
	{
		using namespace Init_RD;
		FType type = *(FType*)addr;
		
		switch (type)
		{
			case F_Folder:
			return readFolder(fs, ((FType*)addr + 1), end);
			
			case F_File:
			return readFile(fs, ((FType*)addr + 1), end);
			
			case F_Special:
			return readSpecial(fs, ((FType*)addr + 1), end);
		}
		
		if (type == F_Folder)
		{
			return readFolder(fs, ((FType*)addr + 1), end);
		}
		else
		{
			return readFile(fs, ((FType*)addr + 1), end);
		}
	}
	
	Node* readFile(InitRD_FS* fs, void* addr, void** endAddr)
	{
		uint64_t span = *(uint64_t*)addr;
		char* recordName = (char*)((uint64_t*)addr + 1);// + sizeof(unsigned long);
		int i = 0;
		while (recordName[i++] != 0x0) ;
		void* data = recordName + i;
		auto size = span - i;
		if (endAddr != nullptr)
		{
			*endAddr = (char*)data + size;
		}

		auto file = new Init_RD::FileNode(fs, recordName, data, size);
		
		return file;
	}
	
	Node* readFolder(InitRD_FS* fs, void* addr, void** end)
	{
		uint64_t span = *(uint64_t*)addr;
		char* recordName = (char*)addr + sizeof(span);
		int i = 0;
		while (recordName[i++] != 0x0) ;
		void* data = recordName + i;
		uint32_t count = *(uint32_t*)data;
		void* entries = (char*)data + sizeof(count);

		auto dir = new Init_RD::DirectoryNode(fs, recordName);
		
		void* ptr = entries;
		void* tmp;
		for (unsigned int j = 0; j < count; ++j)
		{
			Node* entry = readRecord(fs, ptr, &tmp);
			dir->add(entry);
			entry->set_parent(dir);
			ptr = tmp;
		}
		if (end != nullptr)
		{
			*end = ptr;
		}
		return dir;
	}
	
	Node* readSpecial(InitRD_FS* fs, void* addr, void** endAddr)
	{
		using namespace Init_RD;
		SpecialType type = S_Invalid;
		
		
		uint64_t span = *(uint64_t*)addr;
		char* recordName = (char*)((uint64_t*)addr + 1);// + sizeof(unsigned long);
		int i = 0;
		while (recordName[i++] != 0x0) ;
		type = *(SpecialType*)(recordName + i);
		
		void* data = (recordName + i + sizeof(SpecialType));
		
		auto size = span - ((addr_t)data - (addr_t)recordName);
		if (endAddr != nullptr)
		{
			*endAddr = (char*)data + size;
		}
		
		switch (type)
		{
			case S_Function:
			assert(size == 255);
			return new Init_RD::FunctionNode(fs, recordName, data, size);
			
			case S_Invalid:
			return nullptr;
		}
		
		TRACE_VAL((size_t)type);
		
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	
	InitRD_FS::InitRD_FS(void* base) : Filesystem::Filesystem(), _root(nullptr), _base(base), _factory(this)
	{
		assert(base);
		auto r = init_initrd(this, base);
		assert(r);
		assert(r->isKind(NodeType::Directory));
		_root = (Init_RD::DirectoryNode*)r->as_directory();
		//_root = (Init_RD::DirectoryNode*)r;
	}
}
}