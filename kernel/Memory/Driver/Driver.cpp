#include "Driver.hh"
#include <drivers/Device.hh>

#define CHAR_MEM_COUNT 12

namespace Kernel::Memory
{
	Drivers::Device* MemoryDriver::dev_instances[CHAR_MEM_COUNT];
	MemoryDriver MemoryDriver::_instance;
	
	
	
	
	using namespace Drivers;
	
	class MDevice : public Device
	{
		public:
		virtual bool is_char() const noexcept override final
		{ return true; }
	};
	
	
	class Zero : public MDevice
	{
		public:
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			return len;
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			memset((uint8_t*)buf, 0, len);
			return len;
		}
	};
	
	#ifdef TESTING
	
	class DevFwd : public MDevice
	{
		public:
		int error;
		FILE* file;
		
		DevFwd(const char* path) noexcept
		{
			file = fopen(path, "rb+");
			if (!file)
			{
				error = errno;
				perror(path);
				perror(strerror(error));
				//assert(error == EACCES);
			}
		}
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			if (!file)
			{
				errno = error;
				return 0;
			}
			fseek(file, start, SEEK_SET);
			return fread(buf, 1, len, file);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			if (!file)
			{
				errno = error;
				return 0;
			}
			fseek(file, start, SEEK_SET);
			return fwrite(buf, 1, len, file);
		}
		
	};
	
	#endif
	
	#ifdef TESTING
	#define PARENT_DEV DevFwd
	#define PARENT_ARGS(...) __VA_ARGS__
	#else
	#define PARENT_DEV MDevice
	#define PARENT_ARGS(...) 
	#endif
	
	class Random : public PARENT_DEV
	{
		public:
		
		Random() : PARENT_DEV(PARENT_ARGS("/dev/random"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	class URandom : public PARENT_DEV
	{
		public:
		
		URandom() : PARENT_DEV(PARENT_ARGS("/dev/urandom"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	
	class Full : public Zero
	{
		public:
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			errno = ENOSPC;
			return 0;
		}
	};
	
	class Mem : public PARENT_DEV
	{
		public:
		
		Mem() : PARENT_DEV(PARENT_ARGS("/dev/mem"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	
	class KMsg : public PARENT_DEV
	{
		public:
		
		KMsg() : PARENT_DEV(PARENT_ARGS("/dev/kmsg"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	class KMem : public PARENT_DEV
	{
		public:
		
		KMem() : PARENT_DEV(PARENT_ARGS("/dev/kmem"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	class DevNull : public MDevice
	{
		public:
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			return 0;
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			return len;
		}
	};
	
	class Port : public PARENT_DEV
	{
		public:
		
		Port() : PARENT_DEV(PARENT_ARGS("/dev/port"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	class Core : public PARENT_DEV
	{
		public:
		
		Core() : PARENT_DEV(PARENT_ARGS("/dev/core"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	class AIO : public PARENT_DEV
	{
		public:
		
		AIO() : PARENT_DEV(PARENT_ARGS("/dev/aio"))
		{
			
		}
		
		#ifndef TESTING
		
		virtual size_t read(size_t start, size_t len, void* buf) const override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual size_t write(size_t start, size_t len, const void* buf) override
		{
			assert(NOT_IMPLEMENTED);
		}
		
		#endif
	};
	
	
	static struct
	{
		Zero zero;
		Random random;
		Full full;
		URandom urandom;
		Mem mem;
		KMem kmem;
		KMsg kmsg;
		DevNull null;
		Port port;
		Core core;
		AIO aio;
	}
	mem_devs;
	
	
	MemoryDriver::MemoryDriver() noexcept
	{
		for (int i = 0; i < CHAR_MEM_COUNT; ++i)
		{
			dev_instances[i] = nullptr;
		}
		
		dev_instances[CHAR_MEM_ZERO] = &mem_devs.zero;
		dev_instances[CHAR_MEM_RANDOM] = &mem_devs.random;
		dev_instances[CHAR_MEM_FULL] = &mem_devs.full;
		dev_instances[CHAR_MEM_URANDOM] = &mem_devs.urandom;
		dev_instances[CHAR_MEM_MEM] = &mem_devs.mem;
		dev_instances[CHAR_MEM_KMSG] = &mem_devs.kmsg;
		dev_instances[CHAR_MEM_KMEM] = &mem_devs.kmem;
		dev_instances[CHAR_MEM_NULL] = &mem_devs.null;
		dev_instances[CHAR_MEM_PORT] = &mem_devs.port;
		dev_instances[CHAR_MEM_CORE] = &mem_devs.core;
		dev_instances[CHAR_MEM_AIO] = &mem_devs.aio;
		
		
		
		/*dev_instances[CHAR_MEM_ZERO] = new Zero();
		dev_instances[CHAR_MEM_RANDOM] = new Random();
		dev_instances[CHAR_MEM_FULL] = new Full();
		dev_instances[CHAR_MEM_URANDOM] = new URandom();
		dev_instances[CHAR_MEM_MEM] = new Mem();
		dev_instances[CHAR_MEM_KMSG] = new KMsg();*/
		
	}
	
	
	
	
	
	driver_t MemoryDriver::devices() const noexcept
	{
		return CHAR_MEM_COUNT;
	}
	
	Device* MemoryDriver::device(const driver_t id) const noexcept
	{
		if (id >= CHAR_MEM_COUNT)
		{
			return nullptr;
		}
		
		TRACE_VAL(id);
		assert(dev_instances[id]);
		return dev_instances[id];
		assert(NOT_IMPLEMENTED);
	}
}