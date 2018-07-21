#include "Disk.hh"
#include <Utils/detail/functexcept.hh>

namespace Drivers
{
	
	size_t Disk::write(size_t start, uint8_t value, size_t len)
	{
		uint8_t* buf = nullptr;
		__try
		{
			if (len < 256)
			{
				uint8_t buf2[len];
				memset(buf2, value, len);
				return this->write(start, len, buf2);
			}
			else if (len <= 1024)
			{
				buf = new uint8_t[len];
				assert(buf);
				memset(buf, value, len);
				auto res = write(start, len, buf);
				delete[] buf;
				return res;
			}
			else
			{
				buf = new uint8_t[1024];
				assert(buf);
				memset(buf, value, 1024);
				auto res = write(start, 1024, buf);
				len -= 1024;
				start += 1024;
				while (len >= 1024)
				{
					auto tmpRes = write(start, 1024, buf);
					res += tmpRes;
					if (tmpRes < 1024)
					{
						delete[] buf;
						return res;
					}
					start += tmpRes;
					len -= tmpRes;
				}
				
				delete[] buf;
				
				if (len > 0)
				{
					return res + this->write(start, value, len);
				}
				else
				{
					return res;
				}
			}
		}
		__catch(...)
		{
			if (buf)
			{
				delete[] buf;
			}
			__throw_exception_again;
		}
	}
	
}