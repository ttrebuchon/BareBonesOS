#include "QA.hh"
#include <kernel/Filesystem.hh>
#include <kernel/Filesystem/initrd.hh>
#include <fcntl.h>

using namespace Kernel::FS;

bool QA::NativeFileMap(const char* path, const char* native, bool delete_if_exists)
{
	auto fs = Filesystem::Current;
	if (!fs)
	{
		return false;
	}
	
	if (delete_if_exists)
	{
		::unlink(native);
	}
	
	auto initrd = (InitRD_FS*)fs;
	auto n = initrd->create_native_file(path, native);
	return (n != nullptr);
}