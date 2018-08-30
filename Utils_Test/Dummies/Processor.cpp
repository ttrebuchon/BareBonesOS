#include <kernel/Processor.h>

namespace Kernel
{
	extern "C"
	{
		Processor_t* current_processor = nullptr;
	}
}