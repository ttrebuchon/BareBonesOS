#ifndef INCLUDED_HASH_TABLE_HPP
#define INCLUDED_HASH_TABLE_HPP

#include "hash_table.hh"

namespace Utils
{
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::next(detail::hash_node_base* n) -> _VNode*&
	{
		return *((_VNode**)&n->next);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::next(const detail::hash_node_base* n) -> const _VNode* const&
	{
		return (const _VNode* const&)n->next;
	}
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::hash_table(size_t bucketHint, const Selector& select, const hasher& hash, const equals_type& eq, const allocator_type& alloc) : select(select), hash(hash), eq(eq), alloc(alloc), policy(), buckets(nullptr), pre(nullptr), bucketCount(0), elemCount(0), beforeBegin()
	{
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		
		bucketCount = policy.nextBuckets(bucketHint);
		buckets = bAlloc.allocate(bucketCount);
		for (int i = 0; i < bucketCount; ++i)
		{
			buckets[i] = nullptr;
		}
		ASSERT(buckets);
		ASSERT(bucketCount > 0);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::hash_table(const hash_table& t) : select(t.select), hash(t.hash), eq(t.eq), alloc(t.alloc), policy(t.policy), buckets(nullptr), pre(nullptr), bucketCount(t.bucketCount), elemCount(t.elemCount), beforeBegin()
	{
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		
		buckets = bAlloc.allocate(bucketCount);
		for (int i = 0; i < bucketCount; ++i)
		{
			buckets[i] = nullptr;
		}
		
		auto tptr = t._begin();
		auto ptr = allocateNode(tptr->value);
		ptr->code = tptr->code;
		beforeBegin.next = ptr;
		buckets[bucketIndex(ptr)] = (_VNode*)&beforeBegin;
		
		
		detail::hash_node_base* prev = ptr;
		for (tptr = next(tptr); tptr; tptr = next(tptr))
		{
			ptr = allocateNode(tptr->value);
			prev->next = ptr;
			ptr->code = tptr->code;
			size_t bkt = bucketIndex(ptr);
			if (!buckets[bkt])
			{
				buckets[bkt] = (_VNode*)prev;
			}
			prev = ptr;
		}
		
		
		ASSERT(buckets);
		ASSERT(bucketCount > 0);
	}
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::~hash_table()
	{
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		
		_VNode* ptr = _begin();
		_VNode* last = ptr;
		
		if (ptr != nullptr)
		{
			ptr = next(ptr);
			while (ptr)
			{
				ASSERT(last != &beforeBegin);
				nalloc.destroy(last);
				nalloc.deallocate(last, 1);
				
				last = ptr;
				ptr = next(ptr);
			}
			ASSERT(last != &beforeBegin);
			nalloc.destroy(last);
			nalloc.deallocate(last, 1);
		}
		
		bAlloc.deallocate(buckets, bucketCount);
	}
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::getCreate(const Key& k) -> _VNode*
	{
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto node = findNode(index, k, code);
		if (node)
		{
			return node;
		}
		
		node = allocateNode(k);
		
		node->code = code;
		auto needsRehash = policy.needRehash(bucketCount, elemCount, 1);
		if (needsRehash.first)
		{
			rehash(needsRehash.second);
			index = bucketIndex(code);
		}
		
		if (buckets[index])
		{
			next(node) = next(buckets[index]);
			next(buckets[index]) = node;
		}
		else
		{
			next(node) = next(&beforeBegin);
			next(&beforeBegin) = node;
			if (node->next)
			{
				const hash_code nextHash = getCode(next(node));
				buckets[bucketIndex(nextHash)] = node;
			}
			buckets[index] = (_VNode*)&beforeBegin;
		}
		++elemCount;
		return node;
	}
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::getCreate(Key&& k) -> _VNode*
	{
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto node = findNode(index, k, code);
		if (node)
		{
			return node;
		}
		
		node = allocateNode(k);
		
		node->code = code;
		auto needsRehash = policy.needRehash(bucketCount, elemCount, 1);
		if (needsRehash.first)
		{
			rehash(needsRehash.second);
			index = bucketIndex(code);
		}
		
		if (buckets[index])
		{
			next(node) = next(buckets[index]);
			next(buckets[index]) = node;
		}
		else
		{
			next(node) = next(&beforeBegin);
			next(&beforeBegin) = node;
			if (node->next)
			{
				const hash_code nextHash = getCode(next(node));
				buckets[bucketIndex(nextHash)] = node;
			}
			buckets[index] = (_VNode*)&beforeBegin;
		}
		++elemCount;
		return node;
	}
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	size_t hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::bucketIndex(const T& t) const
	{
		return bucketIndex(select(t));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	size_t hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::bucketIndex(const Key& k) const
	{
		return bucketIndex(getCode(k));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	size_t hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::bucketIndex(const hash_code h) const
	{
		return h % bucketCount;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	size_t hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::bucketIndex(const _VNode* n) const
	{
		return bucketIndex(n->code);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::findNode(const size_t bucketIndex, const Key& k, const hash_code c) -> _VNode*
	{
		auto n = findBeforeNode(bucketIndex, k, c);
		if (n)
		{
			return next(n);
		}
		else
		{
			return nullptr;
		}
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::findNode(const size_t bucketIndex, const Key& k, const hash_code c) const -> const _VNode*
	{
		auto n = findBeforeNode(bucketIndex, k, c);
		if (n)
		{
			return next(n);
		}
		else
		{
			return nullptr;
		}
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	detail::hash_node_base* hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::findBeforeNode(const size_t bucketIndex, const Key& k, const hash_code c)
	{
		auto prev_p = buckets[bucketIndex];
		if (!prev_p)
		{
			return nullptr;
		}
		
		auto p = next(prev_p);
		for ( ; ; p = next(p))
		{
			if (equiv(k, p))
			{
				return prev_p;
			}
			if (!p->next || this->bucketIndex(next(p)->code) != bucketIndex)
			{
				break;
			}
			prev_p = p;
		}
		return nullptr;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::allocateNode(const Key& k) const -> _VNode*
	{
		auto ptr = nalloc.allocate(1);
		ptr->next = nullptr;
		ptr->code = 0;
		alloc.construct(&ptr->value, k);
		
		return ptr;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::allocateNode(const T& value) const -> _VNode*
	{
		auto ptr = nalloc.allocate(1);
		ptr->next = nullptr;
		ptr->code = 0;
		alloc.construct(&ptr->value, value);
		
		return ptr;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	void hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::rehash(size_t bCount)
	{
		ASSERT(bCount > 0);
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		auto newBuckets = bAlloc.allocate(bCount);
		for (int i = 0; i < bCount; ++i)
		{
			newBuckets[i] = nullptr;
		}
		ASSERT(newBuckets != nullptr);
		ASSERT(buckets != nullptr);
		ASSERT(bucketCount > 0);
		_VNode* ptr = _begin();
		ASSERT(ptr != nullptr);
		beforeBegin.next = nullptr;
		size_t beginBkt = 0;
		while (ptr)
		{
			_VNode* _next = next(ptr);
			size_t bkt = ptr->code % bCount;
			if (!newBuckets[bkt])
			{
				next(ptr) = next(&beforeBegin);
				beforeBegin.next = ptr;
				newBuckets[bkt] = (_VNode*)&beforeBegin;
				if (ptr->next)
				{
					newBuckets[beginBkt] = ptr;
				}
				beginBkt = bkt;
			}
			else
			{
				next(ptr) = next(newBuckets[bkt]);
				next(newBuckets[bkt]) = ptr;
			}
			ptr = _next;
		}
		bAlloc.deallocate(buckets, bucketCount);
		buckets = newBuckets;
		bucketCount = bCount;
	}
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::_begin() -> _VNode*
	{
		return next(&beforeBegin);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::_begin() const -> const _VNode*
	{
		return next(&beforeBegin);
	}
	
	
	
	
	
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	detail::hash_node<T>* hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::get(const Key& k)
	{
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		ASSERT(false);
	}
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::equiv(const Key& l, const _VNode* r) const
	{
		return eq(l, select(r->value));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::equiv(const Key& l, const Key& r) const
	{
		return eq(l, r);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::equiv(const Key& l, const T& r) const
	{
		return eq(l, select(r));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::equiv(const _VNode* l, const _VNode* r) const
	{
		return eq(select(l->value), select(r->value));
	}
	
	
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::getCode(const T& t) const -> hash_code
	{
		return hash(select(t));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::getCode(const Key& k) const -> hash_code
	{
		return hash(k);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy>::getCode(const _VNode* n) const -> hash_code
	{
		return n->code;
	}
	
	
	
	
}

#endif