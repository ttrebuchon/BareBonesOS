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

template <class T>
list_node_t<T*>* simple_list_remove_nulls(list_node_t<T*>* first)
{
	auto it = first;
	list_node_t<T*>* prev = nullptr;
	while (it)
	{
		if (!it->entity)
		{
			auto next = it->next;
			if (it == first)
			{
				first = next;
			}
			else if (prev)
			{
				prev->next = next;
			}
			
			delete it;
			it = next;
		}
		else
		{
			prev = it;
			it = it->next;
		}
	}
	
	return first;
}

template <class T, class Alloc>
list_node_t<T*>* simple_list_remove_nulls(list_node_t<T*>* first, const Alloc& _alloc = Alloc())
{
	Alloc alloc(_alloc);
	auto it = first;
	list_node_t<T*>* prev = nullptr;
	while (it)
	{
		if (!it->entity)
		{
			auto next = it->next;
			if (it == first)
			{
				first = next;
			}
			else if (prev)
			{
				prev->next = next;
			}
			
			alloc.destroy(it);
			alloc.deallocate(it, 1);
			it = next;
		}
		else
		{
			prev = it;
			it = it->next;
		}
	}
	
	return first;
}


template <class T>
list_node_t<T>* simple_list_last(list_node_t<T>* n)
{
	while (n && n->next)
	{
		n = n->next;
	}
	return n;
}

template <class T>
list_node_t<T>* simple_list_remove(list_node_t<T>* first, list_node_t<T>* target)
{
	assert(first);
	assert(target);
	if (!first)
	{
		return nullptr;
	}
	else if (!target)
	{
		return first;
	}
	
	
	if (target == first)
	{
		auto n = target->next;
		target->next = nullptr;
		return n;
	}
	
	list_node_t<T>* it = first;
	list_node_t<T>* prev = nullptr;
	
	while (it)
	{
		if (it != target)
		{
			prev = it;
			it = it->next;
		}
		else
		{
			prev->next = it->next;
			it->next = nullptr;
			break;
		}
	}
	
	return first;
}

#endif // __cplusplus


END_NS // Collections
END_NS // Support

#endif