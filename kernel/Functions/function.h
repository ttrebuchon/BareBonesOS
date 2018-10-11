#ifndef INCLUDED_KERNEL_FUNCTIONS_FUNCTION_H
#define INCLUDED_KERNEL_FUNCTIONS_FUNCTION_H

#include <Common.h>

#ifdef __cplusplus
namespace Kernel::FS
{
	class DirectoryNode_v;
	
}
#include <Utils/iostream_fwd.hh>
#endif

START_NS(Kernel)
C_CODE




typedef
IF_CPP_ELSE(FS::DirectoryNode_v*, void*)
cwd_type;



typedef struct sys_function
{
	int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&, struct process_info*, const struct sys_function*);
	void* user_data;
	void* sub_fn;
}
sys_function_t;

typedef struct sys_function sys_fn_t;
typedef sys_fn_t sysfn_t;
typedef sys_fn_t sys_func_t;





C_END
END_NS // Kernel

#endif