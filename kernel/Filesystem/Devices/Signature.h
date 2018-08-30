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
__attribute__((__packed__))
dev_t,
dev32_t;

typedef struct ____dev64_t
{
	uint64_t major;
	uint64_t minor;
	
	#ifdef __cplusplus
	constexpr ____dev64_t() noexcept : 
				major(0),
				minor(0)
	{
		
	}
	
	constexpr ____dev64_t(const dev32_t& d) noexcept
			:
				major(d.major),
				minor(d.minor)
	{
		
	}
	
	constexpr explicit operator dev32_t() const noexcept
	{
		assert((~(uint64_t)0xFFFFFFFFULL & major) == 0);
		assert((~(uint64_t)0xFFFFFFFFULL & minor) == 0);
		return dev32_t{
			.major = (uint32_t)major,
			.minor = (uint32_t)minor
		};
	}
	#endif
}
__attribute__((__packed__))
dev64_t;

C_END

END_NS // FS
END_NS // Kernel

#endif