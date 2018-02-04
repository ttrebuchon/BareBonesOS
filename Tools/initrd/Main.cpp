#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>

#define ROOT "FS_ROOT"
#define OUT_IMG "initrd.img"

class Folder;
class File;

enum FType
{
	F_File,
	F_Folder
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
	bool folder = false;
	unsigned long span;
	const char* name;
	unsigned int nameLen;
};

struct FolderRecord : public FRecord
{
	unsigned int count;
	FRecord** entries;
};

struct FileRecord : public FRecord
{
	std::string fileHandle;
};


FolderRecord* packFolder(Folder*);
FileRecord* packFile(File*);

void writePacked(FILE*, FRecord*);













int main()
{
	Folder* root = new Folder(nullptr, "");
	root->stat();
	traverseDir(root);
	
	std::list<FEntry*> entries;
	
	root->getDescendants(entries);
	
	for (auto ent : entries)
	{
		std::cout << ent->getPath();
		if (ent->type() == F_File)
		{
			std::cout << " -- " << ((File*)ent)->size;
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
		if (child->type() == F_Folder)
		{
			children.push_back(packFolder((Folder*)child));
		}
		else if (child->type() == F_File)
		{
			children.push_back(packFile((File*)child));
		}
		else
		{
			std::cerr << "Unknown node type!" << std::endl;
		}
	}
	
	auto rec = new FolderRecord;
	rec->folder = true;
	rec->name = dir->name.c_str();
	rec->nameLen = dir->name.length() + 1;
	rec->span = sizeof(unsigned long) + sizeof(unsigned int) + sizeof(int);
	rec->span += dir->name.length() + 1;
	for (auto c : children)
	{
		rec->span += c->span;
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
	rec->folder = false;
	rec->name = file->name.c_str();
	rec->nameLen = file->name.length() + 1;
	rec->span = sizeof(unsigned long) + sizeof(int);
	rec->span += file->name.length() + 1;
	rec->span += file->size;
	
	rec->fileHandle = ROOT + file->getPath();
	
	return rec;
}


void writeFilePacked(FILE*, FileRecord*);
void writeFolderPacked(FILE*, FolderRecord*);


void writePacked(FILE* out, FRecord* rec)
{
	int fol = rec->folder;
	::fwrite(&fol, 1, sizeof(int), out);
	::fwrite(&rec->span, 1, sizeof(rec->span), out);
	::fwrite(rec->name, rec->nameLen, sizeof(char), out);
	
	
	if (rec->folder)
	{
		writeFolderPacked(out, (FolderRecord*)rec);
	}
	else
	{
		writeFilePacked(out, (FileRecord*)rec);
	}
}


void writeFilePacked(FILE* out, FileRecord* rec)
{
	FILE* in = ::fopen(rec->fileHandle.c_str(), "r");
	
	auto len = rec->span - sizeof(unsigned long) - rec->nameLen - 1 - sizeof(int);
	
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
