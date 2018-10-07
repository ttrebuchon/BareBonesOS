#include <kernel/Filesystem/NTFS/NTFS.hh>
#include <drivers/Disk/Disk.hh>
#include "Tests.hh"
#include <kernel/Filesystem.hh>
#include <Std/errno.h>
#include <kernel/Filesystem/Devices/Signature.h>


static void verify_read();
template <class Fn>
static void enumerate_system_callback(auto root, Fn fn, bool folders);

TEST(ntfs)
{
	verify_read();
}



static void verify_read()
{
	using namespace Kernel::FS;
	auto drive = QA::QACheckReadOnlyDrive("Images/NTFS.img");
	assert(drive);
	assert(drive->capacity() > 0);
	QA::out << "Capacity (MiB): " << ((long double)drive->capacity())/(1024*1024) << std::endl;
	
	
	const size_t disk_sz = drive->capacity();
	
	
	
	{
		auto fs = NTFS::Open(*drive);
		if (!fs)
		{
			if (kerrno != 0)
			{
				auto str = kstrerror(kerrno);
				if (str)
				{
					QA::out << "Error: " << str << std::endl;
				}
			}
		}
			
		assert(fs);
		auto r = fs->root();
		assert(r);
		
		auto rd = r->as_directory();
		assert(rd);
		
		for (int i = 0; i < rd->size(); ++i)
		{
			auto child = rd->at(i);
			assert(child);
			QA::out << "\"" << child->name.c_str() << "\"" << std::endl;
		}
		
		QA::out << "Root: \"" << rd->name.c_str() << "\"" << std::endl;
		
		auto TestDir = rd->at("TestDir");
		assert(TestDir);
		auto TestDir_d = TestDir->as_directory();
		assert(TestDir_d);
		auto sz = TestDir_d->size();
		QA::out << "'TestDir' size: " << sz << std::endl;
		assert(sz == 1);
		auto txtn = TestDir_d->at(0);
		assert(!txtn->isKind(NodeType::Directory));
		for (int i = 0; i < TestDir_d->size(); ++i)
		{
			
		}
		
		auto dev = rd->at("dev");
		assert(dev);
		auto dev_d = dev->as_directory();
		assert(dev_d);
		sz = dev_d->size();
		QA::out << "'dev' size: " << sz << std::endl;
		for (int i = 0; i < dev_d->size(); ++i)
		{
			auto d = dev_d->at(i);
			assert(d);
			auto cdev = d->as_char_device();
			assert(cdev);
			QA::out << rd->name.c_str() << "/" << dev_d->name.c_str() << "/" << cdev->name.c_str() << std::endl;
			
			auto targ = cdev->device();
			assert(targ);
			Kernel::FS::dev_t sig;
			assert(targ->identifiers(nullptr, &sig));
			TRACE_VAL(sig.major);
			TRACE_VAL(sig.minor);
			
			/*uint8_t dat[4];
			assert(cdev->read(0, 4, dat) == 4);*/
		}
		
		
		
		auto txt = txtn->as_file();
		assert(txt);
		
		QA::out << QA::br;
		QA::out << txt->name.c_str() << std::endl;
		QA::out << "size = " << txt->size() << std::endl;
		
		{
			auto s = txt->size();
			char data[s+1];
			assert(txt->read(0, s, (uint8_t*)data) == s);
			data[s] = '\0';
			QA::out << "Data: \"" << data << "\"" << std::endl;
			assert(strcmp(data, "Hello, world!\n") == 0);
		}
		
		{
			auto callback = [](auto root, auto n) -> void
			{
				QA::out << n->get_path().c_str() << std::endl;
				assert(n->get_parent());
				assert(n->get_parent() == root);
				//QA::out << root->get_path().c_str() << "/" << n->name.c_str() << std::endl;
			};
			
			enumerate_system_callback(rd, callback, true);
		}
		
		{
			auto randn = dev_d->at("random");
			assert(randn);
			uint16_t buf[4] = { 0, 0, 0, 0 };
			
			
			auto res = randn->read(0, sizeof(buf), buf);
			assert(res == sizeof(buf));
			
			assert(buf[0] != 0);
			assert(buf[1] != 0);
			assert(buf[2] != 0);
			assert(buf[3] != 0);
			
		}
		
		
		delete fs;
	}
	
	
	
	delete drive;
	
}




template <class Fn>
static void enumerate_system_callback(auto root, Fn fn, bool folders)
{
	using namespace Kernel::FS;
	for (size_t i = 0; i < root->size(); ++i)
	{
		auto n = root->at(i);
		assert(n);
		if (n->isKind(NodeType::Directory, true))
		{
			if (folders)
			{
				fn(root, n);
			}
			auto dir = n->as_directory();
			assert(dir);
			enumerate_system_callback(dir, fn, folders);
		}
	}
	
	for (size_t i = 0; i < root->size(); ++i)
	{
		auto n = root->at(i);
		assert(n);
		if (n->isKind(NodeType::Directory, true) || n->isKind(NodeType::Link))
		{
			continue;
		}
		
		
		
		fn(root, n);
	}
	
	for (size_t i = 0; i < root->size(); ++i)
	{
		auto n = root->at(i);
		assert(n);
		if (!n->isKind(NodeType::Link))
		{
			continue;
		}
		
		
		
		fn(root, n);
	}
}