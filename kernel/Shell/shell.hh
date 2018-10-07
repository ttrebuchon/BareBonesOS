#ifndef INCLUDED_KERNEL_SHELL_SHELL_HH
#define INCLUDED_KERNEL_SHELL_SHELL_HH

#include <Common.h>
#include <kernel/Memory/Allocators/virtual.hh>
#include "result.hh"
#include <Utils/vector>
#include <Utils/string>
#include <Utils/EnumOperators.hh>

namespace Utils
{
	template <class>
	struct Char_Traits;
	
	template <class, class>
	class basic_streambuf;
}

namespace Kernel
{
	namespace detail::shell
	{
		class Command;
	}
	
	namespace FS
	{
		class Node;
		class DirectoryNode_v;
	}
	
	enum Shell_StartOpts : uint32_t
	{
		SHELL_START_DEFAULT = 0x0,
		SHELL_START_NO_FS = 0x1,
		SHELL_START_NO_CWD = 0x2,
		SHELL_START_ALLOC_FS_CONTEXT = 0x4,
		SHELL_START_ALLOC_PROCESS = 0x8,
		SHELL_START_WAIT_CWD = 0x10,
		SHELL_START_EXEC_IN_PROCESS = 0x20,
		SHELL_START_SKIP_DEFAULT_FUNCS = 0x40,
		SHELL_START_NO_INIT_FUNCTIONS = (0x80 | SHELL_START_SKIP_DEFAULT_FUNCS),
		SHELL_START_NO_FUNCTIONS = SHELL_START_NO_INIT_FUNCTIONS,
		
		
		SHELL_START_ALLOC_PROCESS_INFO = 0x100,
	};
	
	ENUM_OPS(Shell_StartOpts, uint32_t);
	
	class Shell
	{
		public:
		typedef Memory::virtual_allocator<uint8_t> allocator_type;
		typedef shell_result_t result_type;
		typedef detail::shell::Command command_type;
		//typedef Utils::basic_string<char, Utils::Char_Traits<char>, typename allocator_type::template rebind<char>::other> string_type;
		typedef Utils::string string_type;
		
		private:
		template <class T>
		using allocator_type_rebind = typename allocator_type::template rebind<T>::other;
		
		typedef Utils::basic_streambuf<char, Utils::Char_Traits<char>> sbuf_type;
		
		
		
		struct process* process;
		allocator_type alloc;
		Utils::vector<string_type, typename allocator_type::template rebind<string_type>::other> path_segments;
		string_type _path_string;
		struct {
			struct filesystem_context* fs_ctx;
			struct process_info* proc_info;
			bool fs_ctx_allocated : 1;
			bool run_in_process : 1;
			bool proc_info_allocated : 1;
		} __attribute__((__packed__));
		class sys_functions* funcs;
		
		
		
		
		Shell(const allocator_type&, struct process* process, Shell_StartOpts opts);
		~Shell();
		
		
		bool parse_internal(command_type*, const char*);
		void set_path_internal(char** segments, size_t count);
		void rebuild_path_string();
		
		bool set_cwd(FS::DirectoryNode_v*);
		bool set_root_filesystem(FS::Filesystem*);
		struct process_info* get_pinfo() const;
		FS::Filesystem* get_root_filesystem() const;
		FS::Filesystem* get_root_filesystem_best_guess() const;
		//result_type execute_simple(FS::Node*, command_type*);
		//result_type execute_simple_in_process(FS::Node*, command_type*, int argc, const char** argv);
		
		result_type execute_simple(FS::Node*, command_type*, sbuf_type* in, sbuf_type* out);
		result_type execute_simple_in_process(FS::Node*, command_type*, int argc, const char** argv, sbuf_type* in, sbuf_type* out);
		
		
		struct process* init_cmd_proc();
		result_type get_error_result(int num, const char*) const;
		result_type get_error_result() const;
		void init_functions(bool register_defaults);
		
		template <class T>
		void dispose(T* t)
		{
			assert(t);
			t->~T();
			typename allocator_type::template rebind<T>::other a(alloc);
			a.deallocate(t, 1);
		}
		
		template <class T>
		T* create(auto... args)
		{
			typename allocator_type::template rebind<T>::other a(alloc);
			T* obj = a.allocate(1);
			assert(obj);
			if (!obj)
			{
				return nullptr;
			}
			new (obj) T(args...);
			return obj;
		}
		
		template <class T>
		allocator_type_rebind<T> get_allocator_for() const
		{
			return allocator_type_rebind<T>(alloc);
		}
		
		sbuf_type* create_sbuf();
		void redirect_sbuf(sbuf_type*);
		void dispose_sbuf(sbuf_type*);
		
		public:
		constexpr static char path_separator = ';';
		
		
		command_type* parse_cmd(const char*);
		result_type execute_cmd(command_type*, sbuf_type* in = nullptr, sbuf_type* out = nullptr);
		result_type execute_cmd(const char* cmd);
		
		void set_path(const char* path_string);
		void set_path(const char** segments, size_t count);
		
		const string_type& path_string() const
		{ return _path_string; }
		
		const string_type* path_segment(const size_t i) const
		{ return (i < path_segments.size() ? &path_segments.at(i) : nullptr); }
		constexpr size_t path_segments_count() const noexcept
		{ return path_segments.size(); }
		
		FS::DirectoryNode_v* get_cwd() const;
		
		void dispose_result(result_type*);
		void dispose_result_output(char*);
		
		friend class ShellModule;
	};
	
}

#endif