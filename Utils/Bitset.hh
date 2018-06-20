#ifndef INCLUDED_BITSET_HH
#define INCLUDED_BITSET_HH

#include <common.h>
#include <debug.h>

START_NS(Utils)


template <uint32_t N, class Type = uchar>
struct Bitset
{
	typedef Type storage;
	constexpr static uint32_t Count = N/(sizeof(Type)*8) + (N%(sizeof(Type)*8) > 0);
	static_assert(Count*sizeof(Type)*8 >= N, "");
	Type bits[Count];
	constexpr static unsigned int Bits = N;
	
	Bitset()
	{
		setAll(false);
	}
	
	void set(const uint32_t index, bool value)
	{
		assert(index < N);
		if (value)
		{
			this->bits[index / (sizeof(Type)*8)] |= (0x1 << (index % (sizeof(Type)*8)));
		}
		else
		{
			this->bits[index / (sizeof(Type)*8)] &= ~(0x1 << (index % (sizeof(Type)*8)));
		}
	}
	
	bool get(const uint32_t index) const
	{
		assert(index < N);
		return (this->bits[index / (sizeof(Type)*8)] >> (index % (sizeof(Type)*8))) & 0x1;
	}
	
	void setAll(bool value)
	{
		if (value)
		{
			for (uint32_t i = 0; i < Count; ++i)
			{
				bits[i] = ~((Type)0);
			}
		}
		else
		{
			for (uint32_t i = 0; i < Count; ++i)
			{
				bits[i] = 0x0;
			}
		}
	}
	
	void invert()
	{
		for (uint32_t i = 0; i < Count; ++i)
		{
			bits[i] ^= ~0;
		}
	}
	
	bool firstTrue(uint32_t* index, const uint32_t start = 0) const
	{
		for (uint32_t i = (start / (sizeof(Type)*8)); i < Count; ++i)
		{
			if ((bits[i] & ~0) > 0)
			{
				for (uint32_t j = 0; j < sizeof(Type)*8 && i*sizeof(Type)*8 + j < N; ++j)
				{
					if (((bits[i] >> j) & 0x1) && (i*sizeof(Type)*8 + j >= start))
					{
						if (index)
						{
							*index = i*sizeof(Type)*8 + j;
						}
						return true;
					}
				}
			}
		}
		return false;
	}
	
	bool firstFalse(uint32_t* index, const uint32_t start = 0) const
	{
		for (uint32_t i = (start / (sizeof(Type)*8)); i < Count; ++i)
		{
			if ((~(bits[i]) & ~0) != 0)
			{
				for (uint32_t j = 0; j < sizeof(Type)*8 && i*sizeof(Type)*8 + j < N; ++j)
				{
					if ((((bits[i] >> j) & 0x1) == 0x0) && (i*sizeof(Type)*8 + j >= start))
					{
						if (index)
						{
							*index = i*sizeof(Type)*8 + j;
						}
						return true;
					}
				}
			}
		}
		return false;
	}
	
	template <uint32_t N2>
	Bitset<N2, Type> resizeTo() const
	{
		Bitset<N2, Type> bs2;
		
		for (int32_t i = 0; i < Count && i < Bitset<N2, Type>::Count; ++i)
		{
			bs2.bits[i] = bits[i];
		}
		
		return bs2;
	}
};



template <class Type = uchar>
struct Bitset_Ptr
{
	public:
	typedef Type storage_type;
	
	
	private:
	uint32_t count;
	
	protected:
	constexpr static size_t ssize = sizeof(storage_type);
	constexpr static size_t sbits = ssize*8;
	constexpr size_t bit_count() const noexcept
	{
		return sbits*count;
	}
	
	public:
	typedef Type storage;
	Type* bits;
	
	
	
	
	class reference
	{
		protected:
		storage_type& item;
		storage_type mask;
		
		reference(storage_type& s, const storage_type mask) noexcept : item(s), mask(mask)
		{}
		
		
		public:
		~reference() = default;
		
		
		operator bool() const noexcept
		{
			return (item & mask) != 0;
		}
		
		reference& operator=(bool x) noexcept
		{
			if (x)
			{
				item |= mask;
			}
			else
			{
				item &= ~mask;
			}
			return *this;
		}
		
		reference& operator=(const reference& rhs) noexcept
		{
			return (*this) = ((bool)rhs);
		}
		
		reference& flip() noexcept
		{
			item &= ~(mask & item);
			return *this;
		}
		bool operator~() const noexcept
		{
			return ~(mask & item) != 0;
		}
		
		friend class Bitset_Ptr;
	};
	
	
	
	
	
	Bitset_Ptr(Type* ptr, uint32_t count) : count(count), bits(ptr)
	{
		setAll(false);
	}
	
	~Bitset_Ptr()
	{
		if (bits)
		{
			delete[] bits;
		}
	}
	
	void set(const uint32_t index, bool value)
	{
		assert(index < count*sizeof(Type)*8);
		if (value)
		{
			this->bits[index / (sizeof(Type)*8)] |= (0x1 << (index % (sizeof(Type)*8)));
		}
		else
		{
			this->bits[index / (sizeof(Type)*8)] &= ~(0x1 << (index % (sizeof(Type)*8)));
		}
	}
	
	bool get(const uint32_t index) const
	{
		assert(index < count*sizeof(Type)*8);
		return (this->bits[index / (sizeof(Type)*8)] >> (index % (sizeof(Type)*8))) & 0x1;
	}
	
	void setAll(bool value)
	{
		if (value)
		{
			for (uint32_t i = 0; i < count; ++i)
			{
				bits[i] = ~((Type)0);
			}
		}
		else
		{
			for (uint32_t i = 0; i < count; ++i)
			{
				bits[i] = 0x0;
			}
		}
	}
	
	void invert()
	{
		for (uint32_t i = 0; i < count; ++i)
		{
			bits[i] ^= ~0;
		}
	}
	
	bool firstTrue(uint32_t* index, const uint32_t start = 0) const
	{
		for (uint32_t i = (start / (sizeof(Type)*8)); i < count; ++i)
		{
			if ((bits[i] & ~0) > 0)
			{
				for (uint32_t j = 0; j < sizeof(Type)*8; ++j)
				{
					if (((bits[i] >> j) & 0x1) && (i*sizeof(Type)*8 + j >= start))
					{
						if (index)
						{
							*index = i*sizeof(Type)*8 + j;
						}
						return true;
					}
				}
			}
		}
		return false;
	}
	
	bool firstFalse(uint32_t* index, const uint32_t start = 0) const
	{
		for (uint32_t i = (start / (sizeof(Type)*8)); i < count; ++i)
		{
			if ((~(bits[i]) & ~0) != 0)
			{
				for (uint32_t j = 0; j < sizeof(Type)*8; ++j)
				{
					if ((((bits[i] >> j) & 0x1) == 0x0) && (i*sizeof(Type)*8 + j >= start))
					{
						if (index)
						{
							*index = i*sizeof(Type)*8 + j;
						}
						return true;
					}
				}
			}
		}
		return false;
	}
	
	template <uint32_t N2>
	Bitset<N2, Type> resizeTo() const
	{
		Bitset<N2, Type> bs2;
		
		for (int32_t i = 0; i < count && i < Bitset<N2, Type>::Count; ++i)
		{
			bs2.bits[i] = bits[i];
		}
		
		return bs2;
	}
	
	void resize(Type* ptr, const size_t nSize) noexcept
	{
		if (count > 0)
		{
			for (size_t i = 0; i < nSize && i < count; ++i)
			{
				ptr[i] = bits[i];
			}
			delete[] bits;
		}
		count = nSize;
		bits = ptr;
	}
	
	void resize(const size_t nSize) noexcept
	{
		size_t count = nSize/sbits;
		if (nSize % sbits != 0)
		{
			++count;
		}
		resize(new Type[count], count);
	}
	
	__attribute__((always_inline))
	constexpr bool empty() const noexcept
	{
		return count == 0;
	}
	
	size_t size() const noexcept
	{
		return bit_count();
	}
	
	
	
	reference operator[](size_t i) noexcept
	{
		return reference(bits[i / sbits], 1 << (i % sbits));
	}
	
	bool operator[](size_t) const noexcept;
};


template <size_t N>
class bitset
{
	public:
	typedef uchar storage_type;
	
	
	class reference
	{
		protected:
		storage_type& item;
		storage_type mask;
		
		reference(storage_type& s, const storage_type mask) noexcept : item(s), mask(mask)
		{}
		
		
		public:
		~reference() = default;
		
		
		operator bool() const noexcept
		{
			return (item & mask) != 0;
		}
		
		reference& operator=(bool x) noexcept
		{
			if (x)
			{
				item |= mask;
			}
			else
			{
				item &= ~mask;
			}
			return *this;
		}
		
		reference& operator=(const reference& rhs) noexcept
		{
			return (*this) = ((bool)rhs);
		}
		
		reference& flip() noexcept
		{
			item &= ~(mask & item);
			return *this;
		}
		bool operator~() const noexcept
		{
			return ~(mask & item) != 0;
		}
		
		friend class bitset;
	};
	
	protected:
	constexpr static size_t ssize = sizeof(storage_type);
	constexpr static size_t sbits = ssize*8;
	constexpr static size_t ucount = N/sbits + (N % sbits == 0 ? 0 : 1);
	storage_type under[ucount];
	
	public:
	
	constexpr bitset() : under()
	{}
	constexpr bitset(unsigned long long);
	
	
	
	constexpr size_t size() const noexcept
	{ return N; }
	size_t count() const noexcept;
	bool any() const noexcept;
	bool all() const noexcept;
	bool none() const noexcept;
	bool test(size_t) const throw();
	bitset& set();
	bitset& set(size_t pos, bool value = true);
	bitset& reset();
	bitset& reset(size_t);
	bitset& flip();
	bitset& flip(size_t);
	
	
	reference operator[](size_t i)
	{
		return reference(under[i / sbits], 1 << (i % sbits));
	}
	
	bool operator[](size_t) const;
	bitset& operator&=(const bitset&) noexcept;
	bitset& operator|=(const bitset&) noexcept;
	bitset& operator^=(const bitset&) noexcept;
	bitset& operator<<=(size_t) noexcept;
	bitset& operator>>=(size_t) noexcept;
	bitset operator~() const noexcept;
	bitset operator<<(size_t) const noexcept;
	bitset operator>>(size_t) const noexcept;
	bool operator==(const bitset&) const noexcept;
	bool operator!=(const bitset&) const noexcept;
};





END_NS


#endif