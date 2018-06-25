#include "slab.h"



static_assert((1 << 15)-1 == 0x7fff);

template <class S>
static unsigned char* slab_allocate_n(S*);

extern "C" {
	
	#define SLAB_INIT_MACRO(N) \
	void initialize_slabs_##N (slabs##N##_t* slabs) \
	{ \
		assert(slabs); \
		slabs->partial_mask = slabs->full_mask = 0; \
		slabs->empty_mask = ~(uint##N##_t)0; \
		\
		for (int i = 0; i < N; ++i) \
		{ \
			auto s = &slabs->s[i]; \
			s->signature = SLAB##N##_SIGNATURE; \
			s->free_count = N-1; \
			s->free_mask = ((uint##N##_t)1 << (N-1))-1; \
		} \
	} \
	
	
	#define SLAB_GEN_MACRO(X) SLAB_INIT_MACRO(X)
	
	#include "slab_sizes.h"
	
	#undef SLAB_GEN_MACRO
	#undef SLAB_INIT_MACRO
	
	
	
	
	unsigned char *slab_allocate(void* slabs)
	{
		assert(slabs);
		auto sig = *(uint32_t*)slabs;
		#define SLAB_GEN_MACRO(N) \
		case SLAB##N##_SIGNATURE: \
		return slab_allocate_n<slabs##N##_t> ((slabs##N##_t*)slabs);
		
		switch (sig)
		{
			#include "slab_sizes.h"
			
			default:
			assert(false);
		}
		
		#undef SLAB_GEN_MACRO
	}
	
	
	
	
	
	
	
}

#define MASK_N(N, index) (1 << (N - 1 - index))


template <class T>
static uint16_t MSB(const T num)
{
	static_assert(sizeof(T)*8 <= 0xFFFF);
	assert(num);
	uint16_t i = 0;
	uint16_t prev, present;
	prev = num;
	present = num << 1;
	while (present > prev)
	{
		prev = present;
		present <<= 1;
		++i;
	}
	return i;
}

static_assert((uint16_t)~(MASK_N(16, 0) | MASK_N(16, 1)) == 0x3FFF);

template <class S>
unsigned char* slab_allocate_n(S* slabs)
{
	constexpr static size_t N = sizeof(slabs->empty_mask)*8;
	
	#define MASK(I) MASK_N(N, I)
	
	if ((slabs->full_mask ^ (~0)) == 0)
	{
		return nullptr;
	}
	else if (slabs->partial_mask)
	{
		uint16_t index = MSB(slabs->partial_mask);
		auto s = &slabs->s[index];
		assert(s->free_count > 0);
		if (--s->free_count == 0)
		{
			slabs->full_mask |= MASK(index);
			slabs->partial_mask &= ~MASK(index);
		}
		
		index = MSB(s->free_mask);
		assert(index > 0);
		
		s->free_mask &= ~MASK(index);
		
		return ((unsigned char*)s) + (PAGE_SIZE / N)*index;
	}
	else
	{
		uint16_t index = MSB(slabs->empty_mask);
		assert(index < N);
		auto s = &slabs->s[index];
		static_assert(sizeof(slabs->s[index]) == PAGE_SIZE);
		
		slabs->partial_mask |= MASK(index);
		slabs->empty_mask &= ~MASK(index);
		
		assert(s->free_count == N-1);
		
		--s->free_count;
		s->free_mask = ~(MASK(0) | MASK(1));
		return ((unsigned char*)(slabs->s + index)) + (PAGE_SIZE / N);
	}
	
	#undef MASK
}