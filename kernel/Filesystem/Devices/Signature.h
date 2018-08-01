#ifndef INCLUDED_FS_DEVICES_SIGNATURE_HH
#define INCLUDED_FS_DEVICES_SIGNATURE_HH

#include <Common.h>

START_NS(Kernel)
START_NS(FS)


C_CODE

typedef struct
{
	uint32_t major;
	uint32_t minor;
}
dev_t;

C_END

END_NS // FS
END_NS // Kernel

#endif