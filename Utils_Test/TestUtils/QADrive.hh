#pragma once
#include <drivers/Disk/Disk.hh>

namespace TestUtils
{
	class QADrive : public Drivers::Disk
	{
		public:
		
		protected:
		const size_t size;
		constexpr static size_t sector_size = 512;
		const size_t sector_count;
		unsigned char* base_address;
		int fd;
		
		
		public:
		QADrive(const char* filename, const size_t size);
		
		virtual ~QADrive();
		
		virtual int read(size_t start, size_t len, unsigned char* buf) const override;
		virtual int write(size_t start, size_t len, const unsigned char* buf) override;
		
		virtual size_t capacity() const noexcept override
		{
			return this->size;
		}
		
	};
	
	class QACheckReadOnlyDrive : public QADrive
	{
		public:
		
		protected:
		
		
		public:
		QACheckReadOnlyDrive(const char* filename, const size_t size);
		
		virtual int write(size_t start, size_t len, const unsigned char* buf) override;
		
	};
}