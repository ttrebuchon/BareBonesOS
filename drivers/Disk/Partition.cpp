#include "Partition.hh"

namespace Drivers
{
	Partition::Partition(Disk& d) noexcept : _device(&d)
	{
		
	}
	
	
	typedef int rw_type;
	
	
	rw_type Partition::read(size_t start, size_t len, uint8_t* buf) const
	{
		assert(len <= capacity());
		assert(capacity()-len >= start);
		if ((len > capacity()) || (capacity()-len < start))
		{
			return 0;
		}
		return _device->read(this->start() + start, len, buf);
	}
	
	rw_type Partition::write(size_t start, size_t len, const uint8_t* buf)
	{
		assert(len <= capacity());
		assert(capacity()-len >= start);
		if ((len > capacity()) || (capacity()-len < start))
		{
			return 0;
		}
		return _device->write(this->start() + start, len, buf);
	}
	
	size_t Partition::write(size_t start, uint8_t value, size_t len)
	{
		assert(len <= capacity());
		assert(capacity()-len >= start);
		if ((len > capacity()) || (capacity()-len < start))
		{
			return 0;
		}
		return _device->write(this->start() + start, value, len);
	}
	
	size_t Partition::capacity() const noexcept
	{
		size_t s, e;
		s = start();
		e = end();
		assert(s <= e);
		if (s >= e)
		{
			return 0;
		}
		
		return e - s;
	}
	
	
	
}