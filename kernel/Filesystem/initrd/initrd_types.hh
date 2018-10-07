#ifndef INCLUDED_FILESYSTEM_INITRD_INITRD_TYPES_HH
#define INCLUDED_FILESYSTEM_INITRD_INITRD_TYPES_HH

#include <Common.h>

namespace Kernel { namespace FS { namespace Init_RD {
	
	enum FType : uint16_t
	{
		F_File = 0x1,
		F_Folder = 0x2,
		F_Special = 0x3,
	};
	
	enum SpecialType : uint16_t
	{
		S_Invalid = 0x0,
		S_Function = 0x1,
	};
	
}
}
}

#endif