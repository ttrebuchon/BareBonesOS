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
		ASSERT(index < N);
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
		ASSERT(index < N);
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
	private:
	uint32_t count;
	
	public:
	typedef Type storage;
	Type* bits;
	
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
		ASSERT(index < count*sizeof(Type)*8);
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
		ASSERT(index < count*sizeof(Type)*8);
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
};







END_NS


#endif