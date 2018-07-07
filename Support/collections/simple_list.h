#ifndef INCLUDED_SUPPORT_COLLECTIONS_SIMPLE_LIST_H
#define INCLUDED_SUPPORT_COLLECTIONS_SIMPLE_LIST_H

#include <Common.h>



C_CODE

typedef struct c_simple_list_node
{
	struct c_simple_list_node *next;
	void* entity;
}
c_list_node_t;

C_END

START_NS(Support)
START_NS(Collections)

#ifdef __cplusplus

template <class T>
struct list_node_t;

template <class T>
struct list_node_t<T*>
{
	list_node_t* next;
	T* entity;
	
	list_node_t() : next(nullptr), entity(nullptr)
	{
		
	}
};

static_assert(alignof(list_node_t<void*>) == alignof(c_list_node_t));
static_assert(sizeof(list_node_t<void*>) == sizeof(c_list_node_t));

#endif // __cplusplus


END_NS // Collections
END_NS // Support

#endif