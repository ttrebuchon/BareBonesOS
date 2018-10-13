#include <kernel/Scheduling/process.h>
#include "shell.hh"
#include "result.hh"
#include "command.hh"
#include "parsing.hh"
#include <kernel/Filesystem.hh>
#include <kernel/Filesystem/Path.hh>
#include <kernel/Scheduling/context.h>
#include <Std/errno.h>
#include <kernel/Filesystem/initrd/FunctionNode.hh>
#include <kernel/Functions/function_manager.hh>
#include <Support/vector_streambuf.hh>

namespace Kernel
{
	Shell::Shell(const allocator_type& alloc, process_t* process, Shell_StartOpts opts) : process(process), alloc(alloc), path_segments(alloc), _path_string(), fs_ctx(nullptr), proc_info(nullptr), fs_ctx_allocated(false), run_in_process(false), proc_info_allocated(false)
	{
		if (!process && HAS_FLAG(opts, SHELL_START_ALLOC_PROCESS))
		{
			assert(NOT_IMPLEMENTED);
		}
		else if (!process && HAS_FLAG(opts, SHELL_START_ALLOC_PROCESS_INFO))
		{
			proc_info = create<struct process_info>();
			proc_info_allocated = true;
		}
		
		if (process && !(opts & SHELL_START_ALLOC_FS_CONTEXT))
		{
			fs_ctx = &process->fs_context;
		}
		else if (get_pinfo() && !(opts & SHELL_START_ALLOC_FS_CONTEXT))
		{
			fs_ctx = &get_pinfo()->fs_context;
		}
		else
		{
			fs_ctx = create<struct filesystem_context>();
			fs_ctx_allocated = true;
		}
		
		if (!fs_ctx->cwd && !(opts & SHELL_START_NO_CWD) && !(opts & SHELL_START_WAIT_CWD))
		{
			// TODO
			// ....
		}
		
		if (HAS_FLAG(opts, SHELL_START_EXEC_IN_PROCESS))
		{
			run_in_process = true;
		}
		
		if (!HAS_FLAG(opts, SHELL_START_NO_FUNCTIONS))
		{
			bool use_defaults = !HAS_FLAG(opts, SHELL_START_SKIP_DEFAULT_FUNCS);
			init_functions(use_defaults);
		}
	}
	
	Shell::~Shell()
	{
		if (fs_ctx_allocated)
		{
			assert(fs_ctx);
			dispose(fs_ctx);
			fs_ctx = nullptr;
			fs_ctx_allocated = false;
		}
		
		if (proc_info_allocated)
		{
			assert(proc_info);
			dispose(proc_info);
			proc_info = nullptr;
			proc_info_allocated = false;
		}
	}
	
	auto Shell::parse_cmd(const char* text) -> command_type*
	{
		assert(text);
		if (!text)
		{
			return nullptr;
		}
		
		auto cmd = create<command_type>(alloc);
		assert(cmd);
		if (!cmd)
		{
			return nullptr;
		}
		
		if (!parse_internal(cmd, text))
		{
			dispose(cmd);
			return nullptr;
		}
		else
		{
			return cmd;
		}
	}
	
	auto Shell::execute_cmd(command_type* cmd, sbuf_type* in, sbuf_type* out) -> result_type
	{
		assert(cmd);
		auto& base = cmd->command;
		
		auto dir = this->get_cwd();
		if (!dir)
		{
			auto fs = FS::Filesystem::Current;
			if (fs)
			{
				auto groot = fs->root();
				if (groot)
				{
					dir = groot->as_directory();
				}
				
				if (dir)
				{
					fs_ctx->fs = fs;
					fs_ctx->cwd = dir;
				}
			}
		}
		
		
		
		assert(dir);
		
		/*for (size_t i = 0; i < dir->size(); ++i)
		{
			TRACE_VAL(dir->at(i)->name.c_str());
			TRACE_VAL((int)dir->at(i)->type());
			if (dir->at(i)->isKind(FS::NodeType::Directory))
			{
				auto subd = dir->at(i)->as_directory();
				for (size_t j = 0; j < subd->size(); ++j)
				{
					TRACE_VAL(dir->at(i)->as_directory()->at(j)->name.c_str());
				}
			}
		}*/
		
		FS::node_ptr<> target = nullptr;
		
		typedef FS::basic_path<typename string_type::value_type, typename string_type::traits_type, typename string_type::allocator_type> path_type;
		
		path_type base_path(base);
		
		if (base_path.parts_length() == 1)
		{
			auto child = dir->at(base_path.part(0));
			if (child)
			{
				target = child;
			}
		}
		else
		{
			assert(base_path.parts_length() > 1);
			if (FS::Path::IsAbsolute(base))
			{
				auto fs = get_root_filesystem_best_guess();
				assert(fs);
				target = fs->getNode(base);
			}
			else
			{
				target = dir->findChild(base_path);
			}
		}
		
		if (target)
		{
			goto found_target;
		}
		
		if (fs_ctx)
		{
			auto fs = fs_ctx->fs;
			
			if (!fs)
			{
				if (fs_ctx->cwd)
				{
					fs = fs_ctx->cwd->get_filesystem();
				}
			}
			
			assert(fs);
			
			for (const auto& path : path_segments)
			{
				auto node = fs->getNode(path);
				if (node)
				{
					if (node->isKind(FS::NodeType::Directory))
					{
						node = fs->getNode(base_path.str(), path);
						if (node)
						{
							target = node;
							break;
						}
					}
					else if (node->name == base_path.part(base_path.parts_length()-1))
					{
						target = node;
						break;
					}
				}
			}
		}
		
		
		
		found_target:
		assert(target);
		
		if (!cmd->continuation)
		{
			return execute_simple(target.get(), cmd, in, out);
			
		}
		else
		{
			auto out1 = create_sbuf();
			execute_simple(target.get(), cmd, in, out1);
			redirect_sbuf(out1);
			auto res = execute_cmd(&cmd->continuation->command, out1, out);
			dispose_sbuf(out1);
			return res;
		}
		
		
		// TODO
		assert(NOT_IMPLEMENTED);
	}
	
	auto Shell::execute_cmd(const char* cmd_text) -> result_type
	{
		auto cmd = parse_cmd(cmd_text);
		assert(cmd);
		auto res = execute_cmd(cmd);
		dispose(cmd);
		return res;
	}
	
	void Shell::set_path(const char* path_str)
	{
		assert(path_str);
		path_segments.clear();
		size_t i, last = 0;
		auto len = strlen(path_str);
		if (!len)
		{
			return;
		}
		
		Utils::vector<string_type, allocator_type_rebind<string_type>> n_segments(get_allocator_for<string_type>());
		if (detail::shell::path_string_parse(path_str, path_str + len, alloc, n_segments))
		{
			path_segments = Utils::move(n_segments);
			rebuild_path_string();
			return;
		}
		
		assert(NOT_IMPLEMENTED);
		
		/*typename string_type::allocator_type c_alloc(alloc);
		
		for (i = 0; i < len; ++i)
		{
			if (path_str[i] == path_separator)
			{
				if (i > last)
				{
					path_segments.emplace_back(&path_str[last], (i - last), c_alloc);
					
				}
				
				last = i + 1;
				continue;
			}
			
			if (path_str[i] == '"' || path_str[i] == '\'')
			{
				char quote = path_str[i];
				size_t j = i + 1;
				
				for (; j < len; ++j)
				{
					if (path_str[j] == quote)
					{
						i = j;
						break;
					}
				}
				if (j == len)
				{
					path_segments.emplace_back(&path_str[last], c_alloc);
					i = j;
					last = len + 1;
					break;
				}
			}
		}
		
		if (last <= i && last < len)
		{
			path_segments.emplace_back(&path_str[last], c_alloc);
		}
		
		_path_string = "";
		_path_string.reserve(len+1);
		for (const auto& str : path_segments)
		{
			_path_string += str + ";";
		}*/
	}
	
	void Shell::dispose_result(result_type* result)
	{
		assert(result);
		if (result->output)
		{
			dispose_result_output(result->output);
			result->output = nullptr;
		}
	}
	
	void Shell::dispose_result_output(char* output)
	{
		assert(output);
		auto char_alloc = get_allocator_for<char>();
		auto len = strlen(output);
		char_alloc.deallocate(output, len + 1);
	}
	
	
	
	
	
	
	
	
	
	
	bool Shell::parse_internal(command_type* cmd, const char* txt)
	{
		return detail::shell::shell_input_parse(txt, txt + strlen(txt), alloc, cmd);
		
	}
	
	void Shell::rebuild_path_string()
	{
		size_t len = 0;
		for (const auto& str : path_segments)
		{
			len += str.length() + 1;
		}
		
		_path_string = "";
		_path_string.reserve(len+1);
		for (const auto& str : path_segments)
		{
			_path_string += str + ";";
		}
	}
	
	FS::DirectoryNode_v* Shell::get_cwd() const
	{
		if (fs_ctx)
		{
			if (unlikely((!fs_ctx->cwd && fs_ctx->fs)))
			{
				auto r = fs_ctx->fs->root();
				if (r)
				{
					return r->as_directory();
				}
				else
				{
					return nullptr;
				}
			}
			
			return fs_ctx->cwd;
		}
		else
		{
			return nullptr;
		}
	}
	
	bool Shell::set_cwd(FS::DirectoryNode_v* dir)
	{
		if (!dir)
		{
			return false;
		}
		
		assert(fs_ctx);
		fs_ctx->cwd = dir;
		return true;
	}
	
	bool Shell::set_root_filesystem(FS::Filesystem* fs)
	{
		if (!fs)
		{
			return false;
		}
		
		assert(fs_ctx);
		fs_ctx->fs = fs;
		return true;
	}
	
	struct process_info* Shell::get_pinfo() const
	{
		if (process)
		{
			return & process->info;
		}
		else if (proc_info)
		{
			return proc_info;
		}
		else
		{
			return nullptr;
		}
	}
	
	FS::Filesystem* Shell::get_root_filesystem() const
	{
		assert(fs_ctx);
		return fs_ctx->fs;
	}
	
	FS::Filesystem* Shell::get_root_filesystem_best_guess() const
	{
		auto fs = get_root_filesystem();
		if (!fs)
		{
			auto cwd = get_cwd();
			if (!cwd)
			{
				return nullptr;
			}
			
			FS::DirectoryNode_v *parent, *gparent;
			parent = cwd->get_parent();
			if (parent)
			{
				gparent = parent->get_parent();
				while (gparent)
				{
					parent = gparent;
					gparent = parent->get_parent();
				}
				fs = parent->get_filesystem();
			}
			
			if (!fs)
			{
				fs = cwd->get_filesystem();
			}
		}
		return fs;
	}
	
	
	
	/*auto Shell::execute_simple(FS::Node* node, command_type* cmd) -> result_type
	{
		assert(node);
		assert(cmd);
		
		const char* args[cmd->args.size() + 1];
		args[0] = cmd->command.c_str();
		for (size_t i = 0; i < cmd->args.size(); ++i)
		{
			args[i+1] = cmd->args[i].c_str();
		}
		
		if (run_in_process || node->isKind(FS::NodeType::Function))
		{
			return execute_simple_in_process(node, cmd, cmd->args.size() + 1, args);
		}
		
		
		
		struct process* proc;
		if (!(proc = init_cmd_proc()))
		{
			return get_error_result();
		}
		
		// TODO
		assert(NOT_IMPLEMENTED);
	}*/
	
	auto Shell::execute_simple(FS::Node* node, command_type* cmd, sbuf_type* in, sbuf_type* out) -> result_type
	{
		assert(node);
		assert(cmd);
		
		const char* args[cmd->args.size() + 1];
		args[0] = cmd->command.c_str();
		for (size_t i = 0; i < cmd->args.size(); ++i)
		{
			args[i+1] = cmd->args[i].c_str();
		}
		
		if (run_in_process || node->isKind(FS::NodeType::Function))
		{
			return execute_simple_in_process(node, cmd, cmd->args.size() + 1, args, in, out);
		}
		
		
		
		struct process* proc;
		if (!(proc = init_cmd_proc()))
		{
			return get_error_result();
		}
		
		// TODO
		assert(NOT_IMPLEMENTED);
	}
	
	/*auto Shell::execute_simple_in_process(FS::Node* node, command_type* cmd, int argc, const char** argv) -> result_type
	{
		assert(node);
		if (node->isKind(FS::NodeType::Function))
		{
			auto fnode = node->as_function();
			assert(fnode);
			
			auto c_str_fn = fnode->function();
			assert(c_str_fn);
			
			Utils::string fn(c_str_fn);
			if (funcs && funcs->count(fn) > 0)
			{
				result_type ret
				{
					.state = SHELL_RESULT_STATE_DONE,
				};
				auto str = funcs->exec_str(fn, argc, argv, get_cwd(), get_pinfo(), &ret.exit_code);
				if (!str.empty())
				{
					str += "\n";
				}
				auto output_alloc = get_allocator_for<char>();
				auto output_len = str.length();
				ret.output = output_alloc.allocate(output_len + 1);
				assert(ret.output);
				if (likely(output_len > 0))
				{
					strncpy(ret.output, str.c_str(), output_len+1);
				}
				ret.output[output_len] = '\0';
				return ret;
			}
			
			// TODO
			assert(NOT_IMPLEMENTED);
		}
		
		
		// TODO
		assert(NOT_IMPLEMENTED);
	}*/
	
	auto Shell::execute_simple_in_process(FS::Node* node, command_type* cmd, int argc, const char** argv, sbuf_type* in, sbuf_type* out) -> result_type
	{
		assert(node);
		if (node->isKind(FS::NodeType::Function))
		{
			auto fnode = node->as_function();
			assert(fnode);
			
			auto c_str_fn = fnode->function();
			assert(c_str_fn);
			
			Utils::string fn(c_str_fn);
			if (funcs && funcs->count(fn) > 0)
			{
				result_type ret
				{
					.state = SHELL_RESULT_STATE_DONE,
				};
				Utils::string str;
				if (out)
				{
					ret.exit_code = funcs->exec(fn, argc, argv, get_cwd(), in, out, get_pinfo());
				}
				else
				{
					str = funcs->exec_str(fn, argc, argv, get_cwd(), in, get_pinfo(), &ret.exit_code);
				}
				
				for (size_t i = 0; i < str.length(); ++i)
				{
					if (str[i] == '\0')
					{
						auto l1 = str.length();
						str = str.erase(i, 1);
						--i;
						assert(str.length() == (l1-1));
					}
				}
				
				if (!str.empty())
				{
					str += "\n";
				}
				
				
				auto output_alloc = get_allocator_for<char>();
				auto output_len = str.length();
				ret.output = output_alloc.allocate(output_len + 1);
				assert(ret.output);
				if (likely(output_len > 0))
				{
					strncpy(ret.output, str.c_str(), output_len+1);
				}
				ret.output[output_len] = '\0';
				return ret;
			}
			
			// TODO
			assert(NOT_IMPLEMENTED);
		}
		
		
		// TODO
		assert(NOT_IMPLEMENTED);
	}
	
	struct process* Shell::init_cmd_proc()
	{
		#ifdef TESTING
		return nullptr;
		#else
		uid_t user = 0;
		if (this->process)
		{
			user = this->process->user;
		}
		struct process* proc;
		if (create_new_process_uid_fs(&proc, user, fs_ctx) != 0)
		{
			return nullptr;
		}
		else
		{
			return proc;
		}
		#endif
		
		/*struct process* proc = create<struct process>();
		
		if (unlikely(!proc))
		{
			return nullptr;
		}
		
		proc->context = create<struct context>();
		if (unlikely(!proc->context))
		{
			dispose(proc);
			return nullptr;
		}
		
		if (this->process)
		{
			proc->user = this->process->user;
		}
		
		
		return proc;
		
		// TODO
		assert(NOT_IMPLEMENTED);*/
	}
	
	auto Shell::get_error_result(int num, const char* msg) const -> result_type
	{
		set_error(num, msg);
		return get_error_result();
	}
	
	auto Shell::get_error_result() const -> result_type
	{
		return result_type {
			.state = SHELL_RESULT_STATE_FAILED,
			.exit_code = 0,
		};
	}
	
	
	
	
	void Shell::init_functions(bool register_defaults)
	{
		funcs = create<sys_functions>();
		assert(funcs);
		if (register_defaults)
		{
			funcs->add_defaults();
		}
	}
	
	auto Shell::create_sbuf() -> sbuf_type*
	{
		return create<Support::vector_streambuf>();
	}
	
	void Shell::redirect_sbuf(sbuf_type* sb)
	{
		Support::vector_streambuf* vsb = (Support::vector_streambuf*)sb;
		vsb->redirect_out_to_in();
	}
	
	void Shell::dispose_sbuf(sbuf_type* sb)
	{
		assert(sb);
		dispose(sb);
	}
}