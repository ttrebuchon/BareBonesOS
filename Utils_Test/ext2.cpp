#include "Tests.hh"
#include <kernel/Filesystem/EXT2/EXT2.hh>
#include <drivers/Disk/Disk.hh>
#include <kernel/Security/UGO.h>
#include <kernel/Filesystem.hh>
#include <kernel/Filesystem/DeviceFileAdapter.hh>
#include <drivers/CHS.h>
#include <kernel/Filesystem/MBR_System_IDs.h>
#include <drivers/Disk/MBRPartition.hh>
#include <kernel/Filesystem/EXT2/EXT2SymLinkNode.hh>

static void verify_dlx();
static void enumerate_system(auto root, Utils::string path = Utils::string(""), bool print_folders = false);

template <class Fn>
static void enumerate_system_callback(auto root, Fn, bool folders);


void abcdef();

static void enumerate_file_disk(const char* filename);

TEST(ext2)
{
	using namespace Kernel::FS;
	
	/*auto ext4 = new EXT2(*QA::QACheckReadOnlyDrive("Images/EXT2.img"));
	assert(ext4->root());
	assert(ext4->root()->as_directory());
	assert(ext4->root()->as_directory()->size() > 0);
	enumerate_system(ext4->root()->as_directory(), "", true);
	
	
	return;*/
	
	enumerate_file_disk("Images/EXT3.img");
	QA::out << QA::hr << QA::br;
	enumerate_file_disk("Images/EXT2.img");
	QA::out << QA::hr << QA::br;
	enumerate_file_disk("Images/minibootable.img");
	QA::out << QA::hr << QA::br;
	
	verify_dlx();
	//return;
	
	
	const uint32_t disk_sz = 16779264;// 20480;
	auto drive = QA::QADrive("Images/EXT2Disk.img", disk_sz);
	assert(drive);
	
	ASSERTEQ(drive->write(0, 0, drive->capacity()), drive->capacity());
	
	/*assert(EXT2::Format(drive));
	{
		auto fs1 = new EXT2(*drive);
		auto r = fs1->root()->as_directory();
			assert(r);
			QA::out << "Got root." << std::endl;
			
			auto test = r->add_directory("Test");
			assert(test);
			assert(test->name == "Test");
			assert(r != test);
			QA::out << "Created /Test/" << std::endl;
			ASSERTEQ(test->size(), 0);
			ASSERTEQ(r->size(), 1);
			
			
			auto test2 = test->add_directory("Test2");
			assert(test2);
			assert(test2->name == "Test2");
			assert(test2 != test);
			QA::out << "Created /Test/Test2/" << std::endl;
			ASSERTEQ(test2->size(), 0);
			ASSERTEQ(test->size(), 1);
			ASSERTEQ(r->size(), 1);
			
			
			auto test3 = test->add_directory("Test3");
			assert(test3);
			assert(test3->name == "Test3");
			assert(test3 != test);
			assert(test3 != test2);
			QA::out << "Created /Test/Test2/Test3/" << std::endl;
			
			enumerate_system(r, "", true);
			//assert(r->size() == 1);
			
			
			//QA::out << "Root Children: " << r->size() << std::endl;
			
			
			auto testf = test3->add_file("Test.txt");
			assert(testf);
			enumerate_system(r, "", true);
			ASSERTEQ(r->size(), 1);
			ASSERTEQ(test->size(), 2);
			ASSERTEQ(test3->size(), 1);
			QA::out << "Created Test.txt!" << std::endl;
			
			enumerate_system(r, "", true);
			
			
			
			auto hndl = testf->handle();
			assert(hndl);
			QA::out << "Got handle!" << std::endl;
			auto f = hndl->file();
			assert(f);
			QA::out << "Got file!" << std::endl;
			
			assert(f->out.rdbuf());
			QA::out << "Verified buffer is present." << std::endl;
			//f->out << "Hello, world!";// << Utils::endl;
			QA::out << "Wrote to buffer!" << std::endl;
	}
	delete drive;
	return;*/
	
	{
		MBR_PTable_t mbr;
		auto p = &mbr.partitions[0];
		
		p->bootable = 0;
		Drivers::CHS_t chs1((1024/512));
		chs1 -= 1;
		
		p->start_head = chs1.head;
		p->start_sector = chs1.sector;
		p->start_cyl = chs1.cylinder;
		
		p->relative_sector = (uint32_t)chs1;
		
		p->system_ID = MBR_SYS_EXT;
		
		Drivers::CHS_t chs2((disk_sz/2)/512);
		chs2 -= 1;
		p->end_head = chs2.head;
		p->end_sector = chs2.sector;
		p->end_cyl = chs2.cylinder;
		
		p->total_sectors = (uint32_t)(chs2 - chs1 + 1);
		
		
		
		p = &mbr.partitions[1];
		
		p->bootable = 0;
		chs1 = Drivers::CHS_t(((disk_sz/2)/512));
		
		p->start_head = chs1.head;
		p->start_sector = chs1.sector;
		p->start_cyl = chs1.cylinder;
		
		p->relative_sector = (uint32_t)chs1;
		
		p->system_ID = MBR_SYS_EXT;
		
		chs2 = Drivers::CHS_t(disk_sz/512);
		chs2 -= 1;
		p->end_head = chs2.head;
		p->end_sector = chs2.sector;
		p->end_cyl = chs2.cylinder;
		
		p->total_sectors = (uint32_t)(chs2 - chs1 + 1);
		
		p = &mbr.partitions[2];
		p->system_ID = 0;
		p = &mbr.partitions[3];
		p->system_ID = 0;
		
		
		MBR_ptable_write(drive, &mbr);
	}
	
	
	{
		auto p1 = Drivers::MBRPartition::Get(drive, 0);
		assert(p1);
		auto p2 = Drivers::MBRPartition::Get(drive, 1);
		assert(p2);
		
		
		QA::out << "P1 Size: " << p1->capacity() << std::endl;
		QA::out << "P2 Size: " << p2->capacity() << std::endl;
		QA::out << "Total: " << (p1->capacity() + p2->capacity()) << std::endl;
		
		QA::out << "P1: [" << p1->start() << ", " << p1->end() << ")" << std::endl;
		QA::out << "P2: [" << p2->start() << ", " << p2->end() << ")" << std::endl;
		
		Drivers::CHS_t chs1((1024/512));
		chs1 -= 1;
		
		Drivers::CHS_t chs2(10240/512);
		chs2 -= 1;
		QA::out << "Correct p1 Size: " << (disk_sz/2)-512 << std::endl;
		
		
		bool res;
		res = EXT2::Format(p1);
		assert(res);
		res = EXT2::Format(p2);
		assert(res);
		
		auto fs1 = new EXT2(*p1);
		{
			auto r = fs1->root()->as_directory();
			assert(r);
			QA::out << "Got root." << std::endl;
			
			//QA::out << r->size() << std::endl;
			
			auto test = r->add_directory("Test");
			assert(test);
			assert(test->name == "Test");
			assert(r != test);
			QA::out << "Created /Test/" << std::endl;
			ASSERTEQ(test->size(), 0);
			ASSERTEQ(r->size(), 1);
			
			
			auto test2 = test->add_directory("Test2");
			assert(test2);
			assert(test2->name == "Test2");
			assert(test2 != test);
			QA::out << "Created /Test/Test2/" << std::endl;
			ASSERTEQ(test2->size(), 0);
			ASSERTEQ(test->size(), 1);
			ASSERTEQ(r->size(), 1);
			
			
			auto test3 = test->add_directory("Test3");
			assert(test3);
			assert(test3->name == "Test3");
			assert(test3 != test);
			assert(test3 != test2);
			QA::out << "Created /Test/Test2/Test3/" << std::endl;
			
			enumerate_system(r, "", true);
			//assert(r->size() == 1);
			
			
			//QA::out << "Root Children: " << r->size() << std::endl;
			
			/*for (auto i = 0; i < r->size(); ++i)
			{
				QA::out << r->at(i)->name.c_str() << std::endl;
			}*/
			
			
			auto testf = test3->add_file("Test.txt");
			assert(testf);
			QA::out << "Created Test.txt!" << std::endl;
			
			ASSERTEQ(r->size(), 1);
			ASSERTEQ(test->size(), 2);
			ASSERTEQ(test3->size(), 1);
			
			enumerate_system(r, "", true);
			
			auto hndl = testf->handle();
			assert(hndl);
			QA::out << "Got handle!" << std::endl;
			auto f = hndl->file();
			assert(f);
			QA::out << "Got file!" << std::endl;
			
			assert(f->out.rdbuf());
			QA::out << "Verified buffer is present." << std::endl;
			//f->out << "Hello, world!";// << Utils::endl;
			QA::out << "Wrote to buffer!" << std::endl;
			
			
		}
		
		QA::out << "Deleting filesystem..." << std::endl;
		delete fs1;
		QA::out << "Deleted filesystem." << std::endl;
		
		QA::out << "Creating filesystem..." << std::endl;
		fs1 = new EXT2(*p1);
		{
			auto r = fs1->root()->as_directory();
			assert(r);
			QA::out << "Got root." << std::endl;
			QA::out << r->size() << std::endl;
			
			enumerate_system(r, "/", true);
			
			
			QA::out << "Root Children: " << r->size() << std::endl;
			
			for (auto i = 0; i < r->size(); ++i)
			{
				QA::out << r->at(i)->name.c_str() << std::endl;
			}
			
			
			
			auto _test = r->at("Test");
			assert(_test);
			auto test = _test->as_directory();
			assert(test);
			
			assert(test->size() == 2);
			enumerate_system(r, "", true);
			
			assert(r != test);
			QA::out << QA::br << "/" << r->name.c_str() << "\n/" << test->name.c_str() << std::endl;
			ASSERTEQ(test->size(), 2);
			auto _test2 = test->at(0);
			assert(_test2);
			auto test2 = _test2->as_directory();
			assert(test2);
			QA::out << "/" << test2->name.c_str() << std::endl;
			assert(test2->size() == 0);
			QA::out << QA::br;
			enumerate_system(r, "", true);
		}
		
		delete fs1;
		fs1 = nullptr;
		
		QA::out << QA::br << QA::hr << std::endl;
		
		p1->write(0, 0, p1->capacity());
		res = EXT2::Format(p1);
		assert(res);
		
		QA::out << "Create filesystem on partition 2..." << std::endl;
		auto fs2 = new EXT2(*p1);
		{
			
				auto r = fs2->root()->as_directory();
				assert(r);
				QA::out << "Got root." << std::endl;
				QA::out << "/" << r->name.c_str() << std::endl;
			
			
			QA::out << "Opening linux filesystem..." << QA::br;
			
			auto drive = QA::QACheckReadOnlyDrive("Images/minibootable.img");
			
			auto linux = new EXT2(*drive);
			QA::out << QA::br << "Linux filesystem opened." << std::endl;
			
			auto fn = [&](auto ldir, auto ln)
			{
				QA::out << std::endl;
				DirectoryNode_v* dir = nullptr;
				assert(ldir);
				assert(linux);
				assert(fs2);
				if (ldir == linux->root())
				{
					QA::out << "Linux root dir." << std::endl;
					auto root = linux->root();
					assert(root);
					QA::out << "/" << root->name.c_str() << std::endl;
					assert(root->isKind(NodeType::Directory));
					
					
					root = fs2->root();
					assert(root);
					assert(root->name.c_str() != nullptr);
					QA::out << "/" << root->name.c_str() << std::endl;
					assert(root->isKind(NodeType::Directory));
					dir = root->as_directory();
					assert(dir);
				}
				else
				{
					auto p = ldir->get_path();
					//QA::out << "'" << p.str().c_str() << "'" << std::endl;
					auto tmp = fs2->getNode(p.str());
					assert(tmp);
					dir = tmp->as_directory();
					assert(dir);
				}
				QA::out << std::endl;
				
				QA::out << ln->get_path().str().c_str() << std::endl;
				
				
				assert(dir);
				auto n = dir->at(ln->name);
				if (!n)
				{
					if (ln->isKind(NodeType::Directory, true))
					{
						n = dir->add_directory(ln->name);
						assert(n);
					}
					else if (ln->isKind(NodeType::File, true))
					{
						auto lfn = ln->as_file();
						assert(lfn);
						n = dir->add_file(ln->name);
						assert(n);
						/*if (lfn->size() < 274432)
						{*/
						QA::out << "\tCopying file (" << lfn->size() << ")..." << std::endl;
						auto buf = new uint8_t[lfn->size()];
						ASSERTEQ(lfn->read(0, lfn->size(), buf), lfn->size());
						ASSERTEQ(n->write(0, lfn->size(), buf), lfn->size());
						delete[] buf;
						/*}
						else
						{
							QA::out << QA::br << QA::hr << std::endl << "File too big!" << std::endl << QA::hr << QA::br;
						}*/
					}
					else if (ln->isKind(NodeType::Link))
					{
						auto lsym = ln->as_link();
						assert(lsym);
						
						auto lsym_ext = (EXT2SymLinkNode*)lsym;
						
						QA::out << "Target: '" << lsym_ext->target_path().c_str() << "'" << std::endl;
						
						
						
						auto ltarg = lsym_ext->target();
						if (!ltarg)
						{
							return;
						}
						
						assert(ltarg);
						
						auto targ = fs2->getNode(ltarg->get_path());
						if (!targ)
						{
							QA::out << "Path: " << ltarg->get_path().c_str() << std::endl;
							assert(NOT_IMPLEMENTED);
						}
						
						n = dir->add_link(ln->name, targ);
					}
					else if (ln->isKind(NodeType::Block, true))
					{
						auto bn = ln->as_block_device();
						assert(bn);
						n = dir->add_block_device(ln->name, bn->device());
						assert(n);
					}
					else if (ln->isKind(NodeType::Char, true))
					{
						auto cn = ln->as_char_device();
						assert(cn);
						n = dir->add_char_device(ln->name, cn->device());
						assert(n);
					}
					else
					{
						QA::out << (int)ln->type() << std::endl;
						assert(NOT_IMPLEMENTED);
					}
				}
				
				assert(n);
			};
			
			QA::out << "Enumerating linux with callback..." << std::endl;
			enumerate_system_callback(linux->root()->as_directory(), fn, true);
			
			QA::out << QA::br << QA::hr << std::endl << "Enumerating linux." << std::endl;
			enumerate_system(linux->root()->as_directory(), "", true);
			
			
			QA::out << QA::hr << std::endl << "Enumerating FS2." << std::endl;
			assert(fs2->root());
			enumerate_system(fs2->root()->as_directory(), "", true);
			
			QA::out << QA::hr << QA::br << "Enumeration Complete." << std::endl;
			
			delete linux;
			delete drive;
		}
		
		delete fs2;
		fs2 = nullptr;
		
		delete p1;
		delete p2;
	}
	
	delete drive;
}


static void enumerate_system(auto root, Utils::string path, bool pfolders)
{
	assert(root);
	using namespace Kernel::FS;
	for (size_t i = 0; i < root->size(); ++i)
	{
		auto n = root->at(i);
		assert(n);
		if (n->isKind(NodeType::Directory))
		{
			if (pfolders)
			{
				QA::out << " + " << path.c_str() << "/" << n->name.c_str() << "/" << std::endl;
			}
			auto dir = n->as_directory();
			assert(dir);
			enumerate_system(dir, path + "/" + n->name, pfolders);
		}
	}
	
	for (size_t i = 0; i < root->size(); ++i)
	{
		auto n = root->at(i);
		assert(n);
		if (n->isKind(NodeType::Directory))
		{
			continue;
		}
		
		QA::out << " + " << path.c_str() << "/" << n->name.c_str() << std::endl;
		
		if (n->isKind(NodeType::File))
		{
			auto fn = n->as_file();
			assert(fn);
			uint8_t sig[4];
			if (fn->size() > 4)
			{
				assert(fn->read(0, 4, sig) == 4);
				if (sig[0] == 0x7F)
				{
					if (sig[1] == 'E' && sig[2] == 'L' && sig[3] == 'F')
					{
						QA::out << "\tELF\n" << std::endl;
					}
				}
				
			}
		}
	}
}


static void verify_dlx()
{
	using namespace Kernel::FS;
	auto drive = QA::QACheckReadOnlyDrive("Images/minibootable.img");
	
	assert(drive);
	assert(drive->capacity() > 0);
	QA::out << "Capacity (MiB): " << ((long double)drive->capacity())/(1024*1024) << std::endl;
	
	
	const size_t disk_sz = drive->capacity();
	
	const auto sb_size = sizeof(detail::EXT2::superblock_ext_t);
	
	unsigned char sb_buf[sb_size];
	drive->read(1024, sb_size, sb_buf);
	
	detail::EXT2::superblock_t* sb = reinterpret_cast<detail::EXT2::superblock_t*>(sb_buf);
	
	
	
	unsigned char sec0[512];
	assert(drive->read(0, 512, sec0) == 512);
	assert(sec0[510] == 0x55);
	assert(sec0[511] == 0xAA);
	
	QA::out << (void*)(addr_t)sb->signature << std::endl;
	assert(sb->signature == detail::EXT2::superblock_constants::ext2_signature);
	
	QA::out << "Version: " << sb->major_ver << "." << sb->minor_ver << std::endl;
	if (sb->major_ver >= detail::EXT2::superblock_constants::ext_major_ver)
	{
		detail::EXT2::superblock_ext_t* sbex = reinterpret_cast<detail::EXT2::superblock_ext_t*>(sb_buf);
		QA::out << (const char*)sbex->volume_name << std::endl;
		QA::out << (const char*)sbex->last_mount_path << std::endl;
		QA::out << (const char*)sbex->journal_ID << std::endl;
	}
	
	
	QA::out << (void*)(addr_t)sb->OS_ID << std::endl;
	QA::out << sb->mounts_since_chk << "/" << sb->mounts_before_chk << " mounts before consistency check" << std::endl;
	QA::out << (1024 << sb->block_size_modifier) << std::endl;
	
	
	
	{
		EXT2 fs(*drive);
		auto r = fs.root();
		assert(r);
		
		QA::out << "Groups: " << fs.group_count() << std::endl;
		QA::out << "Group Size: " << fs.group_size() << std::endl;
		QA::out << "Blocks: " << fs.block_count() << std::endl;
		QA::out << "Block Size: " << fs.block_size() << std::endl;
		QA::out << "Blocks per Group: " << (fs.group_size() / fs.block_size()) << std::endl;
		QA::out << "Free Blocks: " << fs.free_blocks() << std::endl;
		
		const auto& cfs = *&fs;
		auto iroot = cfs.get_inode(2);
		assert(iroot);
		QA::out << iroot->type << std::endl;
		assert(iroot->type & EXT2_INODE_TYPE_DIR);
		QA::out << "Size: " << iroot->size_low << std::endl;
		
		QA::out << "Root Name: '" << fs.root()->name.c_str() << "'" << std::endl;
		
		auto root = fs.root();
		assert(root);
		assert(root->isKind(NodeType::Directory));
		auto droot = root->as_directory();
		
		QA::out << "Root Directory Items: " << droot->size() << std::endl;
		
		for (size_t i = 0; i < droot->size(); ++i)
		{
			auto child = droot->at(i);
			assert(child);
			QA::out << "Child Name: '" << child->name.c_str() << "'" << std::endl;
		}
		
		auto some_child = droot;
		some_child = nullptr;
		for (size_t i = 0; i < droot->size(); ++i)
		{
			auto child = droot->at(i);
			assert(child);
			if (child->isKind(NodeType::Directory))
			{
				if (child->name != "." && child->name != "..")
				{
					some_child = child->as_directory();
					break;
				}
			}
		}
		
		
		assert(some_child);
		
		some_child = droot->at("mnt")->as_directory();
		assert(some_child);
		
		for (size_t i = 0; i < some_child->size(); ++i)
		{
			QA::out << "/" << some_child->name.c_str() << "/" << some_child->at(i)->name.c_str() << std::endl;
		}
		
		QA::out << QA::br << "Fragment Size: " << fs.fragment_size() << QA::br;
		
		QA::out << "Root Name: '" << fs.root()->name.c_str() << "'" << std::endl;
		
		auto sbin_n = droot->at("sbin");
		assert(sbin_n);
		QA::out << "Got sbin." << std::endl;
		QA::out << (unsigned int)sbin_n->type() << std::endl;
		auto sbin_d = sbin_n->as_directory();
		assert(sbin_d);
		assert(sbin_n->isKind(NodeType::Directory));
		auto sbin = sbin_n->as_directory();
		assert(sbin);
		QA::out << "Got sbin as directory." << std::endl;
		{
			auto sz = sbin->size();
			QA::out << "Size: " << sz << std::endl;
			for (size_t i = 0; i < sbin->size(); ++i)
			{
				QA::out << "/" << sbin->name.c_str() << "/" << sbin->at(i)->name.c_str() << std::endl;
			}
		}
		
		
		
		QA::out << QA::br << QA::div << std::endl;
		enumerate_system(droot);
		
		auto lilo_conf = fs.getNode("etc/lilo.conf");
		assert(lilo_conf);
		assert(lilo_conf->isKind(NodeType::File));
		auto lcf = lilo_conf->as_file();
		assert(lcf);
		
		uint8_t buf[lcf->size() + 1];
		
		assert(lcf->size() == lcf->read(0, lcf->size(), buf));
		buf[lcf->size()] = '\0';
		
		QA::out << QA::br << QA::div << std::endl << buf << QA::div << std::endl;
		
		/*auto zimg = fs.getNode("boot/zImage386")->as_file();
		assert(zimg);
		
		auto mnt_n = fs.getNode("/dev/hda");
		assert(mnt_n);
		auto mnt = mnt_n->as_block_device();
		assert(mnt);
		assert(!mnt->has_device());
		auto zimg_dev = new DeviceFileAdapter(zimg);
		mnt->mount(zimg_dev);
		assert(mnt->has_device());
		assert(mnt->device() == zimg_dev);
		
		
		mnt->mount(nullptr);
		assert(!mnt->has_device());
		delete zimg_dev;*/
		
		
		
		auto gr = fs.get_group(0);
		
		for (size_t i = 0; i < gr->inodes_size(); ++i)
		{
			QA::out << "Node " << (i+1) << ": " << gr->node_is_free(i+1) << std::endl;
		}
		
		for (size_t i = 0; i < gr->blocks_size(); ++i)
		{
			if (gr->block_is_free(i))
			{
				QA::out << "Block " << i << ": " << gr->block_is_free(i) << std::endl;
			}
		}
		
		QA::out << QA::br << "First Free Node: " << gr->free_node() << std::endl << "First Free Block: " << gr->free_block() << std::endl;
		
		QA::out << "Directories: " << gr->directories() << std::endl;
		
		/*{
			size_t blk_index;
			auto block = gr->reserve_block(blk_index);
			assert(block);
			
			QA::out << "Index: " << blk_index << std::endl;
			assert(gr->free_block() > blk_index);
			
			assert(gr->release_block(blk_index));
			
			assert(gr->free_block() == blk_index);
		}*/
		
		QA::out << QA::br << QA::br << QA::hr << std::endl;
		enumerate_system_callback(fs.rootd(), [](auto parent, auto node) -> void
		{
			if (node->isKind(NodeType::Block))
			{
				auto n = node->as_block_device();
				if (!n)
				{
					QA::out << (int)node->type() << std::endl;
				}
				assert(n);
				auto dev = n->device();
				assert(dev);
				bool blk = true;
				uint32_t mj = 0, mn = 0;
				assert(dev->identifiers(&blk, &mj, &mn));
				QA::out << n->get_path().c_str() << "\t: (" << (blk ? "Block" : "Char") << ", " << mj << ", " << mn << ")" << std::endl;
			}
			else if (node->isKind(NodeType::Char))
			{
				auto n = node->as_char_device();
				assert(n);
				auto dev = n->device();
				assert(dev);
				bool blk = true;
				uint32_t mj = 0, mn = 0;
				assert(dev->identifiers(&blk, &mj, &mn));
				QA::out << n->get_path().c_str() << "\t: (" << (blk ? "Block" : "Char") << ", " << mj << ", " << mn << ")" << std::endl;
			}
		}, false);
		
		QA::out << QA::hr << QA::br << QA::br << std::endl;
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


static void enumerate_file_disk(const char* filename)
{
	QA::out << filename << std::endl << QA::hr << std::endl;
	auto drive = QA::QACheckReadOnlyDrive(filename);
	auto fs = new Kernel::FS::EXT2(*drive);
	QA::out << "Inode Size: " << fs->inode_size() << std::endl;
	QA::out << "Has Journal? " << fs->has_journal() << std::endl;
	QA::out << "Has Extended Superblock? " << fs->extended_superblock() << std::endl;
	QA::out << "Volume Name: " << fs->volume_name() << std::endl;
	QA::out << "Last Mount Path: " << fs->last_mount_path() << std::endl;
	QA::out << QA::br;
	
	enumerate_system(fs->root()->as_directory(), "", true);
	
	
	
	delete fs;
	delete drive;
}