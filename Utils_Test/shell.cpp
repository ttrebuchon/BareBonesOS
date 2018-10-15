#include "Tests.hh"
#include <kernel/User/User.hh>
#include <Std/errno.h>
#include <Support/sharable.hh>
#include <kernel/Shell/module.hh>
#include <kernel/Shell/shell.hh>
#include <kernel/Scheduling/context.h>
#include <kernel/module.h>
#include <Utils/memory>
#include <kernel/Memory/Allocators/virtual.hh>
#include <kernel/Filesystem/EXT2.hh>
#include <drivers/Disk/Disk.hh>
#include <kernel/Functions/function.hh>
#include <kernel/Filesystem.hh>

static void test_shell_obj(Kernel::Shell*);

TEST(shell)
{
	using namespace Kernel;
	kerrno = 0;
	
	int res;
	
	groupset_t grp;
	groupset_init(&grp);
	
	
	assert(groupset_add_id(&grp, 1) == 0);
	assert(groupset_has_id(&grp, 1) == 1);
	assert(groupset_add_id(&grp, 2) == 0);
	assert(groupset_has_id(&grp, 1) == 1);
	assert(groupset_add_id(&grp, 3) == 0);
	assert(groupset_add_id(&grp, 4) == 0);
	assert(groupset_add_id(&grp, 5) == 0);
	assert(groupset_add_id(&grp, 12) == 0);
	
	assert(groupset_has_id(&grp, 1) == 1);
	assert(groupset_has_id(&grp, 2) == 1);
	assert(groupset_has_id(&grp, 3) == 1);
	assert(groupset_has_id(&grp, 4) == 1);
	assert(groupset_has_id(&grp, 5) == 1);
	assert(groupset_has_id(&grp, 12) == 1);
	
	
	auto gr1 = get_groupset_id(&grp, 0);
	ASSERTEQ(gr1, 1);
	gr1 = get_groupset_id(&grp, 1);
	assert(gr1 == 2);
	gr1 = get_groupset_id(&grp, 2);
	assert(gr1 == 3);
	gr1 = get_groupset_id(&grp, 3);
	assert(gr1 == 4);
	gr1 = get_groupset_id(&grp, 4);
	assert(gr1 == 5);
	
	auto gr2 = get_groupset_id(&grp, 5);
	assert(gr2 == 12);
	
	
	assert(groupset_add_id(&grp, 20) == 0);
	assert(groupset_add_id(&grp, 100) == 0);
	
	gr2 = get_groupset_id(&grp, 6);
	assert(gr2 == 20);
	
	
	auto gr3 = get_groupset_id(&grp, 7);
	assert(gr3 == 100);
	
	
	assert(groupset_get_groups(&grp, 0, nullptr) == 8);
	
	
	mod_info_fn mifn = nullptr;
	mifn = &ShellModule::module_info;
	
	
	mod_info_t* mi = new mod_info_t;
	assert(mifn(mi) != nullptr);
	mi->name[MAX_MODULE_NAME_LENGTH] = '\0';
	
	QA::out << "Module Name: \"" << mi->name << "\"" << std::endl;
	
	auto heap_alloc = new Memory::virtual_allocator<uint8_t>(Memory::virtual_allocator<uint8_t>::make(Utils::allocator<uint8_t>()));
	
	uint8_t* obj = nullptr;
	void* rmod = nullptr;
	ShellModule* mod = nullptr;
	
	if (mi->setup_object_size > 0)
	{
		obj = new uint8_t[mi->setup_object_size];
	}
	
	mod_init_args_t* margs = new mod_init_args_t;
	memset(margs, 0, sizeof(mod_init_args_t));
	margs->heap_alloc = heap_alloc;
	
	assert(mi->open);
	rmod = mi->open(obj, margs);
	mod = (ShellModule*)rmod;
	
	delete margs->heap_alloc;
	delete margs;
	margs = nullptr;
	heap_alloc = nullptr;
	
	
	auto root_drive = QA::QACheckReadOnlyDrive("Images/minibootable.img");
	auto fs = new FS::EXT2(*root_drive);
	
	assert(FS::Filesystem::Current);
	auto shell = mod->start(uid_t(0), FS::Filesystem::Current/*->root()->as_directory()*/);
	
	assert(FS::Filesystem::Current->root().as_directory()->at("var")->get_parent());
	
	
	//auto shell = mod->start(uid_t(0), fs->root()->as_directory());
	assert(shell);
	shell->set_path("/bin;/usr/bin;/sbin;/usr/sbin;'/user programs';/'some; path;';;;/lib");
	QA::out << "Shell Path: " << shell->path_string().c_str() << std::endl;
	for (size_t i = 0; i < shell->path_segments_count(); ++i)
	{
		QA::out << shell->path_segment(i)->c_str() << std::endl;
	}
	test_shell_obj(shell);
	res = mod->stop(shell);
	assert(res == 0);
	
	
	
	assert(mi->close);
	mi->close(rmod);
	
	if (obj)
	{
		delete[] obj;
		obj = nullptr;
	}
	delete mi;
	
	delete fs;
	delete root_drive;
}



static void test_shell_obj(Kernel::Shell* shell)
{
	assert(shell);
	
	//auto res = shell->execute_cmd("cat file.txt test2 'test3 test4' | cat 'hello world' file5.txt");
	//auto res = shell->execute_cmd("cat file.txt test2 'test3 test4' \"test5 test6     test7\"        test8 || cat arg1 arg2 arg3 'compound args1' & ls *.txt | echo");
	
	shell->set_env_var("XYZ", "Hello,");
	shell->set_env_var("ABC", "world!");
	assert((shell->get_env_var("XYZ") + " " + shell->get_env_var("ABC")) == "Hello, world!");
	
	auto res = shell->execute_cmd("ls");
	//auto res = shell->execute_cmd("cat file.txt test2 'test3 test4' \"test5 test6     test7\"        test8");
	assert(res.exit_code == 0);
	
	
	const char* const_prefix = "dev@home:/";
	std::string prefix = const_prefix;
	prefix += "~/ $ ";
	
	{
		auto rel = Kernel::FS::Path::MakeRelative("/", Kernel::FS::Path::Root);
		QA::out << "Relative: " << rel.c_str() << std::endl;
	}
	
	auto set_prefix = [&prefix, const_prefix](auto shell)
	{
		prefix = const_prefix;
		prefix += shell->get_cwd()->get_path().c_str();
		prefix += " $ ";
	};
	assert(Kernel::shell_result_is_good(&res));
	QA::out << std::endl << "\n\n" << std::endl;
	set_prefix(shell);
	QA::out << prefix << "ls" << std::endl;
	QA::out << res.output << std::flush;
	shell->dispose_result(&res);
	
	
	
	std::string in;
	while (true)
	{
		in = "";
		while (in == "")
		{
			set_prefix(shell);
			QA::out << prefix << std::flush;
			std::getline(std::cin, in);
		}
		if (in == "exit")
		{
			break;
		}
		res = shell->execute_cmd(in.c_str());
		assert(Kernel::shell_result_is_good(&res));
		if (res.output)
		{
			QA::out << res.output << std::flush;
		}
		shell->dispose_result(&res);
	}
	
	QA::out << std::endl;
}