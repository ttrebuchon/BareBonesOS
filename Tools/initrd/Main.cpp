#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include "../../Libraries/json/json.hpp"
#include <fstream>

#define ROOT "FS_ROOT"
#define OUT_IMG "initrd.img"

class Folder;
class File;

enum FType : uint16_t
{
	F_File = 0x1,
	F_Folder = 0x2,
	F_Special = 0x3,
};

enum SpecialType : uint16_t
{
	S_Invalid = 0x0,
	S_Function = 0x1,
	
};

class FEntry
{
	public:
	std::string name;
	Folder* parent;
	
	FEntry(Folder* parent, std::string name);
	
	virtual std::string getPath() const;
	
	virtual FType type() const = 0;
	virtual void stat() = 0;
};

class File : public FEntry
{
	public:
	unsigned long size;
	
	File(Folder* parent, std::string name) : FEntry(parent, name), size(0)
	{ }
	
	virtual FType type() const
	{
		return F_File;
	}
	
	virtual void stat()
	{
		const std::string path = ROOT + getPath();
		struct stat st;
		if (::stat(path.c_str(), &st) == 0)
		{
			size = st.st_size;
		}
	}
};

class Folder : public FEntry
{
	public:
	std::vector<FEntry*> children;
	
	Folder(Folder* parent, std::string name) : FEntry(parent, name), children()
	{}
	
	virtual FType type() const
	{
		return F_Folder;
	}
	
	virtual void stat()
	{
		/*const std::string path = ROOT + getPath();
		struct stat st;
		if (::stat(path.c_str(), &st) == 0)
		{
			
		}*/
	}
	
	template <class T>
	void getDescendants(T& t)
	{
		for (auto child : children)
		{
			t.push_back(child);
			if (child->type() == F_Folder)
			{
				((Folder*)child)->getDescendants(t);
			}
		}
	}
	
	template <class T>
	void getDescendants(T& t, const FType ftype)
	{
		for (auto child : children)
		{
			if (child->type() == ftype)
			{
				t.push_back(child);
			}
			if (child->type() == F_Folder)
			{
				((Folder*)child)->getDescendants(t, ftype);
			}
		}
	}
	
};

class Special : public FEntry
{
	public:
	SpecialType special_type;
	uint32_t data_size;
	void* data;
	
	
	Special(Folder* parent, std::string name) : FEntry(parent, name), special_type(S_Invalid), data_size(0), data(nullptr)
	{ }
	
	virtual FType type() const
	{
		return F_Special;
	}
	
	virtual void stat()
	{
		
	}
};




FEntry::FEntry(Folder* parent, std::string name) : name(name), parent(parent)
{
	if (parent != nullptr)
	{
		parent->children.push_back(this);
	}
}


std::string FEntry::getPath() const
{
	if (parent != nullptr)
	{
		return parent->getPath() + "/" + name;
	}
	return name;
}

void traverseDir(Folder*);

struct FRecord
{
	FType type;
	uint64_t span;
	const char* name;
	uint32_t nameLen;
};

struct FolderRecord : public FRecord
{
	uint32_t count;
	FRecord** entries;
};

struct FileRecord : public FRecord
{
	std::string fileHandle;
};

struct SpecialRecord : public FRecord
{
	SpecialType kind;
	uint32_t data_length;
	void* data;
};


FolderRecord* packFolder(Folder*);
FileRecord* packFile(File*);
SpecialRecord* packSpecial(Special*);

void writePacked(FILE*, FRecord*);








nlohmann::json read_defs(const char* defs_filename)
{
	std::ifstream in(defs_filename);
	std::string text = std::string(std::istream_iterator<char>(in), std::istream_iterator<char>());
	return nlohmann::json::parse(text);
}


void insert_defs(Folder* root, nlohmann::json& defs);


int main()
{
	Folder* root = new Folder(nullptr, "");
	root->stat();
	traverseDir(root);
	
	nlohmann::json defs = read_defs("defs.json");
	insert_defs(root, defs);
	
	std::list<FEntry*> entries;
	
	root->getDescendants(entries);
	
	for (auto ent : entries)
	{
		std::cout << ent->getPath();
		if (ent->type() == F_File)
		{
			std::cout << " -- " << ((File*)ent)->size;
			assert(((File*)ent)->size == 13);
		}
		std::cout << "\n";
	}
	std::cout << std::flush;
	
	
	auto packed = packFolder(root);
	
	std::cout << "Data Packed.\n";
	std::cout << "Root record span: " << packed->span << "\n";
	FILE* out = ::fopen(OUT_IMG, "w");
	writePacked(out, packed);
	::fclose(out);
	std::cout << "Done writing." << std::endl;
	
}















void traverseDir(Folder* folder)
{
	DIR* dp;
	dirent* ent;
	std::string path = ROOT + folder->getPath();
	
	if ((dp = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dp)) != NULL)
		{
			if (ent->d_name[0] == '.')
			{
				continue;
			}
			
			if (ent->d_type == DT_DIR)
			{
				auto dir = new Folder(folder, ent->d_name);
				dir->stat();
				traverseDir(dir);
			}
			else if (ent->d_type == DT_REG)
			{
				auto f = new File(folder, ent->d_name);
				f->stat();
			}
		}
		closedir(dp);
	}
}







FolderRecord* packFolder(Folder* dir)
{
	std::vector<FRecord*> children;
	for (auto child : dir->children)
	{
		FRecord* rec = nullptr;
		if (child->type() == F_Folder)
		{
			rec = packFolder((Folder*)child);
		}
		else if (child->type() == F_File)
		{
			rec = packFile((File*)child);
		}
		else if (child->type() == F_Special)
		{
			rec = packSpecial((Special*)child);
		}
		else
		{
			std::cerr << "Unknown node type!" << std::endl;
		}
		
		if (rec)
		{
			children.push_back(rec);
		}
	}
	
	auto rec = new FolderRecord;
	rec->type = F_Folder;
	rec->name = dir->name.c_str();
	rec->nameLen = dir->name.length() + 1;
	//rec->span = sizeof(unsigned long) + sizeof(unsigned int) + sizeof(FType);
	rec->span = sizeof(rec->count);
	rec->span += dir->name.length() + 1;
	for (auto c : children)
	{
		rec->span += (c->span + sizeof(FType) + sizeof(c->span));
	}
	rec->entries = new FRecord*[rec->count = children.size()];
	for (int i = 0; i < children.size(); ++i)
	{
		rec->entries[i] = children[i];
	}
	return rec;
}

FileRecord* packFile(File* file)
{
	auto rec = new FileRecord;
	rec->type = F_File;
	rec->name = file->name.c_str();
	rec->nameLen = file->name.length() + 1;
	rec->span = file->name.length() + 1;
	rec->span += file->size;
	
	rec->fileHandle = ROOT + file->getPath();
	
	return rec;
}

SpecialRecord* packSpecial(Special* spec)
{
	SpecialRecord* rec = nullptr;
	rec = new SpecialRecord;
	rec->type = F_Special;
	rec->kind = spec->special_type;
	rec->name = spec->name.c_str();
	rec->nameLen = spec->name.length() + 1;
	rec->span = rec->nameLen;
	rec->span += sizeof(spec->special_type);
	rec->span += spec->data_size;
	
	rec->data_length = spec->data_size;
	rec->data = spec->data;
	
	return rec;
}


void writeFilePacked(FILE*, FileRecord*);
void writeFolderPacked(FILE*, FolderRecord*);
void writeSpecialPacked(FILE*, SpecialRecord*);


void writePacked(FILE* out, FRecord* rec)
{
	uint16_t type = rec->type;
	::fwrite(&type, 1, sizeof(uint16_t), out);
	::fwrite(&rec->span, 1, sizeof(rec->span), out);
	::fwrite(rec->name, rec->nameLen, sizeof(char), out);
	
	
	
	switch (rec->type)
	{
		case F_Folder:
		writeFolderPacked(out, (FolderRecord*)rec);
		break;
		
		case F_File:
		writeFilePacked(out, (FileRecord*)rec);
		break;
		
		case F_Special:
		writeSpecialPacked(out, (SpecialRecord*)rec);
		break;
	}
	
}


void writeFilePacked(FILE* out, FileRecord* rec)
{
	FILE* in = ::fopen(rec->fileHandle.c_str(), "r");
	
	auto len = rec->span - rec->nameLen/* - 1*/;
	
	auto data = malloc(len);
	::fread(data, 1, len, in);
	::fclose(in);
	::fwrite(data, 1, len, out);
	free(data);
}

void writeFolderPacked(FILE* out, FolderRecord* rec)
{
	::fwrite(&rec->count, 1, sizeof(rec->count), out);
	for (int i = 0; i < rec->count; ++i)
	{
		writePacked(out, rec->entries[i]);
	}
}

void writeSpecialPacked(FILE* out, SpecialRecord* rec)
{
	assert(rec->kind != S_Invalid);
	
	::fwrite(&rec->kind, 1, sizeof(rec->kind), out);
	
	auto len = rec->span - rec->nameLen - sizeof(rec->kind);
	
	if (rec->data)
	{
		::fwrite(rec->data, rec->data_length, sizeof(uint8_t), out);
	}
	else
	{
		assert(rec->data_length == 0);
	}
}


void insert_def(Folder* parent, const std::string& name, nlohmann::json& def);


void insert_defs(Folder* root, nlohmann::json& defs)
{
	bool found;
	for (auto it = defs.begin(); it != defs.end(); ++it)
	{
		start_loop:
		if (it->count("type"))
		{
			insert_def(root, it.key(), *it);
			continue;
		}
		found = false;
		for (auto f : root->children)
		{
			if (f->name == it.key())
			{
				if (f->type() == F_Folder)
				{
					insert_defs((Folder*)f, *it);
					found = true;
					break;
				}
				else
				{
					assert(false);
				}
			}
		}
		
		if (found)
		{
			continue;
		}
		
		new Folder(root, it.key());
		goto start_loop;
		
		
		std::cout << it.key() << std::endl;
	}
}


void insert_def(Folder* parent, const std::string& name, nlohmann::json& def)
{
	std::cout << "Creating def " << name << "..." << std::endl;
	if (def.at("type") == "special")
	{
		auto& data = def["data"];
		auto spec = new Special(parent, name);
		auto kind = data["kind"];
		if (kind == "function")
		{
			spec->special_type = S_Function;
			
			auto n_data = new char[255];
			std::string fn_name = name;
			if (data.count("name"))
			{
				fn_name = data["name"];
			}
			
			assert(fn_name.length() < 255);
			assert(fn_name.length() > 0);
			strcpy(n_data, fn_name.c_str());
			
			
			spec->data = n_data;
			spec->data_size = 255;
			
		}
		else
		{
			std::cout << "Unknown special kind." << std::endl;
			assert(false);
		}
	}
	else if (def.at("type") == "file")
	{
		assert(false);
	}
	else
	{
		std::cout << "Unknown def type." << std::endl;
		assert(false);
	}
}