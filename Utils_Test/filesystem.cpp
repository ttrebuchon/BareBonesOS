#include "Tests.hh"
#include <kernel/Filesystem.hh>

TEST(filesystem)
{
	using namespace Kernel;
	using namespace Kernel::FS;
	
	auto fs = Filesystem::Current;
	assert(fs);
	
	auto r = fs->root();
	assert(r);
	assert(r->isKind(NodeType::Directory));
	
	auto c0 = ((DirectoryNode*)fs->root())->at(0);
	assert(c0);
	
	assert(c0->isKind(NodeType::File));
	
	QA::out << c0->name.c_str() << std::endl;
	
	auto f0 = (FileNode*)c0;
	assert(!f0->inUse());
	
	{
	
	auto fh = f0->handle();
	assert(fh);
	
	assert(f0->inUse());
	assert(!f0->handle());
	
	auto file = fh->file();
	assert(file);
	
	
	Utils::string contents;
	ASSERTEQ(f0->size(), 13);
	char* buf = new char[f0->size()+1];
	buf[f0->size()] = '\0';
	file->in.read(buf, f0->size());
	
	contents = buf;
	assert(contents == "Hello, world!");
	
	std::cout << buf << std::endl;
	
	delete[] buf;
	
	}
	assert(!f0->inUse());
	assert(f0->handle());
	
	{
	
	auto fh = f0->handle();
	assert(fh);
	
	assert(f0->inUse());
	assert(!f0->handle());
	
	auto file = fh->file();
	assert(file);
	
	
	Utils::string contents;
	ASSERTEQ(f0->size(), 13);
	char* buf = new char[f0->size()+1];
	buf[f0->size()] = '\0';
	file->in.read(buf, f0->size());
	
	contents = buf;
	assert(contents == "Hello, world!");
	
	std::cout << buf << std::endl;
	
	delete[] buf;
	
	}
	
	
	
}
	
