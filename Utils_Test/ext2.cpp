#include "Tests.hh"
#include <kernel/Filesystem/EXT2/EXT2.hh>
#include <drivers/Disk/Disk.hh>
#include <kernel/Security/UGO.h>
#include <kernel/Filesystem.hh>
#include <kernel/Filesystem/DeviceFileAdapter.hh>
#include <drivers/CHS.h>
#include <kernel/Filesystem/MBR_System_IDs.h>
#include <drivers/Disk/MBRPartition.hh>

static void verify_dlx();



TEST(ext2)
{
	using namespace Kernel::FS;
	
	verify_dlx();
	
	
	const uint32_t disk_sz = 16779264;// 20480;
	auto drive = QA::QADrive("EXT2Disk.img", disk_sz);
	assert(drive);
	
	
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
		
		
		bool res = EXT2::Format(p1);
		assert(res);
		res = EXT2::Format(p2);
		assert(res);
		
		auto fs1 = new EXT2(*p1);
		{
			auto r = fs1->root()->as_directory();
			assert(r);
			
			QA::out << r->size() << std::endl;
			
			
		}
		
		delete fs1;
		delete p1;
		delete p2;
	}
	
	delete drive;
}


static void enumerate_system(auto root, Utils::string path = Utils::string(""))
{
	using namespace Kernel::FS;
	for (size_t i = 0; i < root->size(); ++i)
	{
		auto n = root->at(i);
		assert(n);
		if (n->isKind(NodeType::Directory))
		{
			//QA::out << path.c_str() << "/" << n->name.c_str() << "/" << std::endl;
			auto dir = n->as_directory();
			assert(dir);
			enumerate_system(dir, path + "/" + n->name);
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
		
		QA::out << path.c_str() << "/" << n->name.c_str() << std::endl;
		
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
	auto drive = QA::QADrive("minibootable.img");
	
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
	else
	
	
	QA::out << (void*)(addr_t)sb->OS_ID << std::endl;
	QA::out << sb->mounts_since_chk << "/" << sb->mounts_before_chk << " mounts before consistency check" << std::endl;
	QA::out << (1024 << sb->block_size_modifier) << std::endl;
	
	
	
	{
		EXT2 fs(*drive);
		auto r = fs.root();
		
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
		/*auto bin = droot->at("bin");
		assert(bin);*/
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
		
		auto zimg = fs.getNode("boot/zImage386")->as_file();
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
		delete zimg_dev;
		
		
	}
	
	
	
	delete drive;
}