#include "QADrive.hh"
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "QA.hh"
#include <drivers/Driver.hh>
#include <sys/stat.h>

namespace TestUtils
{
	
	QADrive::QADrive(const char* filename, const size_t size) : size(size % sector_size == 0 ? size : ((size / sector_size + 1)*sector_size)), sector_count(this->size / sector_size), base_address(nullptr), fd(-1)
	{
		size_t exist_size;
		fd = ::open(filename, O_RDWR | O_APPEND | O_CREAT);
		assert(fd >= 0);
		exist_size = ::lseek(fd, 0, SEEK_END);
		if (exist_size < this->size)
		{
			if (::ftruncate(fd, this->size) != 0)
			{
				assert(false);
			}
		}
		lseek(fd, 0, SEEK_SET);
		
		/*FILE* f = ::fopen(filename, "a+b");
		assert(f);
		fseek(f, 0, SEEK_END);
		exist_size = ftell(f);
		if (exist_size < this->size)
		{
			fseek(f, 0, SEEK_BEG);
			if (::truncate(filename, this->size) != 0)
			{
				assert(false);
			}
		}
		fclose(f);
		f = nullptr;
		if (exist_size < this->size)
		{
			f = ::fopen(filename, "a+b");
			assert(f);
			unsigned char z = 0;
			for (int i = 0; i < this->size - exist_size; ++i)
			{
				::fwrite(&z, 1, 1, f);
			}
			
			fclose(f);
			f = nullptr;
		}
		fd = ::open(filename, O_RDWR | O_APPEND | O_CREAT);
		assert(fd >= 0);*/
		void* phys = ::mmap(0, this->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		assert(phys != MAP_FAILED);
		assert(phys != nullptr);
		this->base_address = (unsigned char*)phys;
	}
	
	QADrive::~QADrive()
	{
		int res = munmap(base_address, size);
		assert(res == 0);
		res = ::close(fd);
		assert(res == 0);
	}
	
	
	int QADrive::read(size_t start, size_t len, unsigned char* buf) const
	{
		if (start + len >= size)
		{
			QA::out << "[" << start << ", " << (start+len) << "]" << std::endl;
		}
		assert(start + len <= size);
		memcpy(buf, base_address + start, len);
		return len;
	}
	
	int QADrive::write(size_t start, size_t len, const unsigned char* buf)
	{
		assert(start + len <= size);
		memcpy(base_address + start, buf, len);
		return len;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	QACheckReadOnlyDrive::QACheckReadOnlyDrive(const char* filename, const size_t size) : QADrive(filename, size)
	{
		
	}
	
	
	int QACheckReadOnlyDrive::write(size_t start, size_t len, const unsigned char* buf)
	{
		assert(start + len < size);
		for (size_t i = 0; i < len; ++i)
		{
			if (base_address[start + i] != buf[i])
			{
				QA::out << "Value mismatch at " << (const void*)(uintptr_t)(start + i) << " :  " << std::endl;
			}
			assert(base_address[start + i] == buf[i]);
		}
		return len;
	}
	
	
	
	
	
	
	
	
	
	using namespace Drivers;
	
	class QADriver : public Driver
	{
		public:
		
		virtual driver_t devices() const noexcept
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual Device* device(const driver_t) const noexcept
		{
			assert(NOT_IMPLEMENTED);
		}
		
		virtual driver_t major() const noexcept
		{
			return 384;
		}
	};
	
	
	
	
	
	
	
	static QADriver qa_driver;
	Driver* QADrive::Driver() noexcept
	{
		// TODO
		return &qa_driver;
	}
}