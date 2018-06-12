#include "CPUID.h"
#include <Std/stdlib.h>

extern "C" {
	
	unsigned char* cpuid_string(uint32_t);
}

namespace Drivers { namespace CPUID
{
extern "C" {
	
	uint32_t cpuid_cmd(uint32_t code, uint32_t* ax)
	{
		uint32_t _ax;
		uint32_t _dx;
		#ifdef __ENV_x86__
		asm volatile("cpuid" : "=a"(_ax), "=d"(_dx) : "a"(code): "ecx", "ebx");
		#elif defined(TESTING)
		
		if (code == 0x1)
		{
			_dx = ((uint32_t)0)-1;
		}
		
		#else
		
		#error Unknown architecture
		
		#endif
		if (ax)
		{
			*ax = _ax;
		}
		return _dx;
	}
	
	
	
}
	
	Utils::string cpuid_string(uint32_t code)
	{
		unsigned char* cstr = ::cpuid_string(code);
		if (cstr)
		{
			Utils::string str(reinterpret_cast<char*>(cstr));
			free(cstr);
			return str;
		}
		else
		{
			return Utils::string();
		}
	}

}
}


extern "C" 
unsigned char* cpuid_string(uint32_t code)
{
	unsigned char* buf = (unsigned char*)::malloc(sizeof(unsigned char)*13);
	
	uint32_t regs[4]; //ax, bx, dx, cx
	static_assert(3*sizeof(uint32_t) == 12*sizeof(unsigned char));
	
	#ifdef __ENV_x86__
	asm volatile("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=d"(regs[2]), "=c"(regs[3]) : "a"(code));
	#elif defined(TESTING)
	{
		unsigned char tmp1[4] = {'T', 'e', 's', 't'};
		unsigned char tmp2[4] = {' ', ' ', ' ', 'C'};
		unsigned char tmp3[4] = {'P', 'U', 'I', 'D'};
		
		regs[1] = *reinterpret_cast<uint32_t*>(tmp1);
		regs[2] = *reinterpret_cast<uint32_t*>(tmp2);
		regs[3] = *reinterpret_cast<uint32_t*>(tmp3);
	}
	#else
	
	#error Unknown architecture
	
	#endif
	
	memcpy(buf, reinterpret_cast<unsigned char*>(&regs[1]), 12);
	buf[12] = '\0';
	
	return buf;
}