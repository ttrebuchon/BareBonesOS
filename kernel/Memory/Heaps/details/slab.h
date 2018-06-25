#ifndef INCLUDED_MEMORY_HEAPS_SLAB_H
#define INCLUDED_MEMORY_HEAPS_SLAB_H

#include <Common.h>

C_CODE


#ifdef SLABS_DEF_MACRO
	#error SLABS_DEF_MACRO is already defined!
#endif

#define SLABS_DEF_MACRO(N) \
typedef struct { \
	uint32_t signature; \
	uint##N##_t free_count; \
	uint##N##_t free_mask; \
	\
	unsigned char free_space[PAGE_SIZE - sizeof(uint32_t) - 2*sizeof(uint##N##_t)]; \
	\
} \
__attribute((__packed__)) \
slab##N##_t; \
\
typedef struct slabs##N##_type \
{ \
	uint##N##_t empty_mask; \
	uint##N##_t partial_mask; \
	uint##N##_t full_mask; \
	slab##N##_t __attribute__((__aligned__(PAGE_SIZE))) \
	s[N]; \
} \
slabs##N##_t; \
void initialize_slabs_##N (slabs##N##_t*);


#ifdef SLAB_GEN_MACRO
	#error SLABS_GEN_MACRO is already defined!
#endif

#define SLAB_GEN_MACRO(X) SLABS_DEF_MACRO(X)

#include "slab_sizes.h"

#undef SLAB_GEN_MACRO


/*typedef struct {
	uint32_t signature;
	uint16_t free_count;
	uint16_t free_mask;
	
	unsigned char free_space[PAGE_SIZE - 8];
} slab16_t; 


typedef struct slabs_type
{
	uint16_t empty_mask;
	uint16_t partial_mask;
	uint16_t full_mask;
	slab16_t __attribute__((__aligned__(16*PAGE_SIZE)))
	s[16];
}
slabs16_t;*/


unsigned char *slab_allocate(void* slabs);
int slab_release(void* slabs, unsigned char* addr);
//void initialize_slabs(void* slabs, );


#undef SLABS_DEF_MACRO

C_END


#define SLAB16_SIGNATURE 0x51AB51AB
#define SLAB32_SIGNATURE 0xA5A5A5A5
#define SLAB64_SIGNATURE 0xBAADF00D

#define RELEASE_OK 0x0
#define RELEASE_BAD_ADDRESS 0x1
#define RELEASE_ALREADY_FREE 0x2

#endif