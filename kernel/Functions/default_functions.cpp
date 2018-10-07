#include "function_manager.hh"
#include <kernel/Filesystem.hh>
#include <Support/threading/Mutex.hh>
#include "function.hh"
#include <Std/errno.h>
#include <kernel/Scheduling/process.h>


#define DEFAULTS_SECTION __attribute__((section("kernel_default_functions")))


namespace Kernel
{
	struct labeled_default
	{
		sys_function_t fn;
		const char* label;
	};
	
	
	
	static int ls(int argc, const char** argv, cwd_type cwd, Utils::ostream& out);
	static int cd(int argc, const char** argv, cwd_type cwd, Utils::ostream&, struct process_info*);
	static int echo(int argc, const char** argv, Utils::ostream&);
	static int pwd(int argc, const char** argv, cwd_type cwd, Utils::ostream&);
	static int cat(int argc, const char** argv, cwd_type cwd, Utils::istream&, Utils::ostream&);
	
	static labeled_default default_fns[] DEFAULTS_SECTION =
	{
		{
			create_system_fn(&ls),
			"ls"
		},
		{
			create_system_fn(&cd),
			"cd"
		},
		{
			create_system_fn(&echo),
			"echo"
		},
		{
			create_system_fn(&pwd),
			"pwd"
		},
		{
			create_system_fn(&cat),
			"cat"
		}
	};
	
	
	void sys_functions::add_defaults()
	{
		size_t default_n = sizeof(default_fns)/sizeof(labeled_default);
		for (size_t i = 0; i < default_n; ++i)
		{
			this->add_fn(default_fns[i].label, default_fns[i].fn);
		}
	}
	
	
	
	
	
	
	
	
	
	static int ls_dir(cwd_type dir, Utils::ostream& out)
	{
		assert(dir);
		// TODO
		
		auto n = dir->size();
		for (size_t i = 0; i < n; ++i)
		{
			out << dir->at(i)->name << "\n";
		}
		
		return 0;
	}
	
	static int ls(int argc, const char** argv, cwd_type cwd, Utils::ostream& out)
	{
		if (!cwd)
		{
			return -1;
		}
		
		if (argc > 1)
		{
			
			auto fs = FS::Filesystem::Current;
			assert(fs);
			int res = 0;
			FS::Node* n = nullptr;
			FS::DirectoryNode_v* dir = nullptr;
			for (int i = 1; i < argc && res == 0; ++i)
			{
				
				if (FS::Path::IsAbsolute(argv[i]))
				{
					n = fs->getNode(argv[i]);
				}
				else
				{
					n = fs->getNode(argv[i], cwd->get_path().str());
				}
				
				if (!n)
				{
					const char* LS_NO_ENT = "No such file or directory.";
					res = ENOENT;
					set_error(res, LS_NO_ENT);
					out << LS_NO_ENT;
					break;
				}
				
				
				assert(n);
				dir = n->as_directory();
				if (!dir)
				{
					const char* LS_NOT_DIR = "Is not a directory.";
					res = ENOTDIR;
					set_error(res, LS_NOT_DIR);
					out << LS_NOT_DIR;
					break;
				}
				
				assert(dir);
				res = ls_dir(dir, out);
			}
			return res;
		}
		else
		{
			return ls_dir(cwd, out);
		}
	}
	
	static int cd(int argc, const char** argv, cwd_type cwd, Utils::ostream& out, struct process_info* proc)
	{
		if (!proc)
		{
			tset_error(EINVAL, "Non null process_info must be provided to 'cd'.");
			return EINVAL;
		}
		
		if (argc <= 0)
		{
			tset_error(EINVAL, "'cd': argc should always be >= 1.");
			return EINVAL;
		}
		
		if (argc == 1)
		{
			return 0;
		}
		else if (unlikely(argc > 2))
		{
			out << "cd: Too many arguments";
			return 0;
		}
		
		const char* target = argv[1];
		if (strlen(target) == 0)
		{
			return 0;
		}
		
		if (strcmp(target, ".") == 0)
		{
			return 0;
		}
		else if (strcmp(target, "..") == 0)
		{
			auto node = proc->fs_context.cwd;
			if (node)
			{
				auto nnode = node->get_parent();
				if (nnode)
				{
					node = nnode->as_directory();
					if (node)
					{
						proc->fs_context.cwd = node;
					}
				}
			}
			
			return 0;
		}
		
		FS::Node* node = nullptr;
		
		if (FS::Path::IsAbsolute(target))
		{
			auto fs = proc->fs_context.fs;
			if (!fs)
			{
				tset_error(EINVAL, "process_info provided to cd has no filesystem.");
				return EINVAL;
			}
			node = fs->getNode(target);
		}
		else
		{
			assert(cwd);
			node = cwd->findChild(target);
		}
		
		if (!node)
		{
			out << "cd: No such directory";
			return ENOENT;
		}
		else if (!node->isKind(FS::NodeType::Directory))
		{
			out << "cd: Is not a directory";
			return ENOTDIR;
		}
			
		proc->fs_context.cwd = node->as_directory();
		return 0;
	}
	
	static int echo(int argc, const char** argv, Utils::ostream& out)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (i > 1)
			{
				out << " ";
			}
			out << argv[i];
		}
		
		return 0;
	}
	
	static int pwd(int argc, const char** argv, cwd_type cwd, Utils::ostream& out)
	{
		if (!cwd)
		{
			tset_error(EINVAL, "nullptr 'cwd' provided to pwd.");
			return EINVAL;
		}
		
		out << cwd->get_path().str();
		return 0;
	}
	
	static int cat(int argc, const char** argv, cwd_type cwd, Utils::istream& in, Utils::ostream& out)
	{
		if (argc > 1)
		{
			for (int i = 1; i < argc; ++i)
			{
				// TODO
				assert(NOT_IMPLEMENTED);
			}
		}
		else
		{
			in >> out.rdbuf();
		}
		return 0;
	}
	
	
}