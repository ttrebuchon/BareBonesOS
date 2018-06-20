#ifndef INCLUDED_HASH_TABLE_HPP
#define INCLUDED_HASH_TABLE_HPP

#include "hash_table.hh"

namespace Utils
{
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::next(detail::hash_node_base* n) -> _VNode*&
	{
		return *((_VNode**)&n->next);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::next(const detail::hash_node_base* n) -> const _VNode* const&
	{
		return (const _VNode* const&)n->next;
	}
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::hash_table(size_type bucketHint, const Selector& select, const hasher& hash, const equals_type& eq, const allocator_type& alloc, bool check_conflicts) : select(select), hash(hash), eq(eq), alloc(alloc), _policy(), resolver(), _check_conflicts(check_conflicts), buckets(nullptr), pre(nullptr), bucketCount(0), elemCount(0), beforeBegin()
	{
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		
		bucketCount = _policy.nextBuckets(bucketHint);
		buckets = bAlloc.allocate(bucketCount);
		for (int i = 0; i < bucketCount; ++i)
		{
			buckets[i] = nullptr;
		}
		assert(buckets);
		assert(bucketCount > 0);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::hash_table(const hash_table& t) : select(t.select), hash(t.hash), eq(t.eq), alloc(t.alloc), _policy(t._policy), resolver(t.resolver), _check_conflicts(t.check_conflicts()), buckets(nullptr), pre(nullptr), bucketCount(t.bucketCount), elemCount(
	#ifdef DEBUG
	0
	#else
	t.elemCount
	#endif
	), beforeBegin()
	{
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		
		buckets = bAlloc.allocate(bucketCount);
		for (int i = 0; i < bucketCount; ++i)
		{
			buckets[i] = nullptr;
		}
		
		auto tptr = t._begin();
		auto ptr = this->allocate_node_value(nalloc, tptr->value);
		ptr->code = tptr->code;
		beforeBegin.next = ptr;
		buckets[bucketIndex(ptr)] = (_VNode*)&beforeBegin;
		#ifdef DEBUG
		++elemCount;
		#endif
		
		
		detail::hash_node_base* prev = ptr;
		for (tptr = next(tptr); tptr; tptr = next(tptr))
		{
			ptr = this->allocate_node_value(nalloc, tptr->value);
			prev->next = ptr;
			ptr->code = tptr->code;
			size_t bkt = bucketIndex(ptr);
			if (!buckets[bkt])
			{
				buckets[bkt] = (_VNode*)prev;
			}
			prev = ptr;
			#ifdef DEBUG
			++elemCount;
			#endif
		}
		
		
		assert(buckets);
		assert(bucketCount > 0);
		assert(elemCount == t.elemCount);
	}
	
	
	/*template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::hash_table(hash_table&& t) : select(Utils::move(t.select)), hash(Utils::move(t.hash)), eq(Utils::move(t.eq)), alloc(Utils::move(t.alloc)), _policy(Utils::move(t._policy)), resolver(Utils::move(t.resolver)), _check_conflicts(t.check_conflicts()), buckets(Utils::move(t.buckets)), pre(Utils::move(t.pre)), bucketCount(Utils::move(t.bucketCount)), elemCount(Utils::move(t.elemCount)), beforeBegin(Utils::move(t.beforeBegin))
	{
		for (auto it = begin(); it != end(); ++it)
		{
			if (it.node == &t.beforeBegin)
			{
				it.node = (_VNode*)&this->beforeBegin;
			}
		}
		
		t.buckets = nullptr;
		t.bucketCount = 0;
		t.pre = nullptr;
		t.elemCount = 0;
		
		
		assert(buckets);
		assert(bucketCount > 0);
	}*/
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::~hash_table()
	{
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		
		/*_VNode* ptr = _begin();
		_VNode* last = ptr;
		
		if (ptr != nullptr)
		{
			ptr = next(ptr);
			while (ptr)
			{
				assert(last != &beforeBegin);
				nalloc.destroy(last);
				nalloc.deallocate(last, 1);
				
				last = ptr;
				ptr = next(ptr);
			}
			assert(last != &beforeBegin);
			nalloc.destroy(last);
			nalloc.deallocate(last, 1);
		}*/
		clear();
		
		bAlloc.deallocate(buckets, bucketCount);
	}
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::operator=(const hash_table& t) -> hash_table&
	{
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		
		clear();
		
		bucket_alloc_type oldBAlloc(alloc);
		
		oldBAlloc.deallocate(buckets, bucketCount);
		
		select = t.select;
		hash = t.hash;
		eq = t.eq;
		alloc = t.alloc;
		_policy = t._policy;
		bucketCount = t.bucketCount;
		#ifndef DEBUG
		elemCount = t.elemCount;
		#else
		elemCount = 0;
		#endif
		beforeBegin = t.beforeBegin;
		
		
		bucket_alloc_type bAlloc(alloc);
		
		
		buckets = bAlloc.allocate(bucketCount);
		for (int i = 0; i < bucketCount; ++i)
		{
			buckets[i] = nullptr;
		}
		
		auto tptr = t._begin();
		auto ptr = this->allocate_node_value(nalloc, tptr->value);
		ptr->code = tptr->code;
		beforeBegin.next = ptr;
		buckets[bucketIndex(ptr)] = (_VNode*)&beforeBegin;
		#ifdef DEBUG
		++elemCount;
		#endif
		
		
		detail::hash_node_base* prev = ptr;
		for (tptr = next(tptr); tptr; tptr = next(tptr))
		{
			ptr = this->allocate_node_value(nalloc, tptr->value);
			prev->next = ptr;
			ptr->code = tptr->code;
			size_t bkt = bucketIndex(ptr);
			if (!buckets[bkt])
			{
				buckets[bkt] = (_VNode*)prev;
			}
			prev = ptr;
			#ifdef DEBUG
			++elemCount;
			#endif
		}
		
		
		assert(buckets);
		assert(bucketCount > 0);
		assert(elemCount == t.elemCount);
		
		
		return *this;
	}
	
	
	
	
	
	/*template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::getCreate(const Key& k, bool* created) -> _VNode*
	{
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto node = findNode(index, k, code);
		if (node)
		{
			if (created)
			{
				*created = false;
			}
			return node;
		}
		
		node = this->allocate_node_key(nalloc, k);
		
		node->code = code;
		auto needsRehash = _policy.needRehash(bucketCount, elemCount, 1);
		if (needsRehash.first)
		{
			_rehash(needsRehash.second);
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
		if (created)
		{
			*created = true;
		}
		return node;
	}*/
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::getCreate(const T& t, bool* created) -> _VNode*
	{
		auto k = select(t);
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto node = findNode(index, k, code);
		if (node)
		{
			if (created)
			{
				*created = false;
			}
			return node;
		}
		
		node = this->allocate_node_value(nalloc, t);
		
		
		node->code = code;
		auto needsRehash = _policy.needRehash(bucketCount, elemCount, 1);
		if (needsRehash.first)
		{
			_rehash(needsRehash.second);
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
				const hash_code nextHash = getCode_node(next(node));
				buckets[bucketIndex(nextHash)] = node;
			}
			buckets[index] = (_VNode*)&beforeBegin;
		}
		++elemCount;
		if (created)
		{
			*created = true;
		}
		return node;
	}
	
	
	/*template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::getCreate(Key&& k, bool* created) -> _VNode*
	{
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto node = findNode(index, k, code);
		if (node)
		{
			if (created)
			{
				*created = false;
			}
			return node;
		}
		
		node = this->allocate_node_key(nalloc, forward<Key&&>(k));
		
		node->code = code;
		auto needsRehash = _policy.needRehash(bucketCount, elemCount, 1);
		if (needsRehash.first)
		{
			_rehash(needsRehash.second);
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
		if (created)
		{
			*created = true;
		}
		return node;
	}*/
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::getCreate(T&& _t, bool* created) -> _VNode*
	{
		T t(forward<T&&>(_t));
		Key k = select(t);
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto node = findNode(index, k, code);
		if (node)
		{
			if (created)
			{
				*created = false;
			}
			return node;
		}
		
		node = this->allocate_node_value(nalloc, forward<T&&>(move(t)));
		
		node->code = code;
		auto needsRehash = _policy.needRehash(bucketCount, elemCount, 1);
		if (needsRehash.first)
		{
			_rehash(needsRehash.second);
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
				const hash_code nextHash = getCode_node(next(node));
				buckets[bucketIndex(nextHash)] = node;
			}
			buckets[index] = (_VNode*)&beforeBegin;
		}
		++elemCount;
		if (created)
		{
			*created = true;
		}
		return node;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::insertNode(_VNode* node)
	{
		if (!node)
		{
			return false;
		}
		
		auto& key = select(node->value);
		auto code = getCode_key(key);
		node->code = code;
		next(node) = nullptr;
		size_t index = bucketIndex(node->code);
		
		
		
		auto needsRehash = _policy.needRehash(bucketCount, elemCount, 1);
		if (needsRehash.first)
		{
			_rehash(needsRehash.second);
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
				const hash_code nextHash = getCode_node(next(node));
				buckets[bucketIndex(nextHash)] = node;
			}
			buckets[index] = (_VNode*)&beforeBegin;
		}
		++elemCount;
		
		return true;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::insertValue(T&& t) -> _VNode*
	{
		_VNode* node = this->allocate_node_value(nalloc, forward<T&&>(t));
		insertNode(node);
		return node;
		
		/*auto& key = select(node->value);
		auto code = getCode(key);
		node->code = code;
		auto index = bucketIndex(node);
		assert(index < bucketCount);
		
		auto& bucket = buckets[index];
		if (!bucket)
		{
			bucket = node;
			++elemCount;
			return node;
		}
		
		
		next(node) = next(bucket);
		next(bucket) = node;
		
		return node;*/
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::deleteNode(_VNode* n)
	{
		assert(n);
		auto index = bucketIndex(n->code);
		auto n_prev = buckets[index];
		
		
		
		while (next(n_prev) != n)
		{
			n_prev = next(n_prev);
			if (!next(n_prev))
			{
				return false;
			}
		}
		
		auto n_next = next(n);
		next(n_prev) = n_next;
		--elemCount;
		this->deallocate_node(nalloc, n);
		return true;
	}
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::bucketIndex_value(const T& t) const -> size_type
	{
		return bucketIndex_key(select(t));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::bucketIndex_key(const Key& k) const -> size_type
	{
		return bucketIndex(getCode_key(k));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::bucketIndex(const hash_code h) const -> size_type
	{
		return h % bucketCount;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::bucketIndex(const _VNode* n) const -> size_type
	{
		return bucketIndex(n->code);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::findNode(const size_type bucketIndex, const Key& k, const hash_code c) -> _VNode*
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
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::findNode(const size_type bucketIndex, const Key& k, const hash_code c) const -> const _VNode*
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
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	detail::hash_node_base* hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::findBeforeNode(const size_type bucketIndex, const Key& k, const hash_code c)
	{
		auto prev_p = buckets[bucketIndex];
		if (!prev_p)
		{
			return nullptr;
		}
		
		auto p = next(prev_p);
		for ( ; ; p = next(p))
		{
			if (equiv_kn(k, p))
			{
				if (check_conflicts())
				{
					auto prev_right = p;
					auto check_p = p;
					while (check_p = next(prev_right))
					{
						if (this->bucketIndex(check_p->code) != bucketIndex)
						{
							break;
						}
						if (equiv_kn(k, check_p))
						{
							if (resolve(buckets[bucketIndex], k, c, prev_p, p, prev_right, check_p) < 0)
							{
								assert(false);
								return findBeforeNode(bucketIndex, k, c);
							}
							else if (next(prev_p) == p)
							{
								return prev_p;
							}
							else
							{
								assert(false);
								return findBeforeNode(bucketIndex, k, c);
							}
						}
						prev_right = check_p;
					}
				}
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
	
	/*template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::allocateNode(const Key& k) const -> _VNode*
	{
		auto ptr = nalloc.allocate(1);
		ptr->next = nullptr;
		ptr->code = 0;
		alloc.construct(&ptr->value, k);
		
		return ptr;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::allocateNode(const T& value) const -> _VNode*
	{
		auto ptr = nalloc.allocate(1);
		ptr->next = nullptr;
		ptr->code = 0;
		alloc.construct(&ptr->value, value);
		
		return ptr;
	}*/
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	void hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::_rehash(size_type bCount)
	{
		assert(bCount > 0);
		typedef typename allocator_type::template rebind<Bucket>::other bucket_alloc_type;
		bucket_alloc_type bAlloc(alloc);
		
		auto newBuckets = bAlloc.allocate(bCount);
		for (int i = 0; i < bCount; ++i)
		{
			newBuckets[i] = nullptr;
		}
		assert(newBuckets != nullptr);
		assert(buckets != nullptr);
		assert(bucketCount > 0);
		_VNode* ptr = _begin();
		assert(ptr != nullptr);
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
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::_begin() -> _VNode*
	{
		return next(&beforeBegin);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::_begin() const -> const _VNode*
	{
		return next(&beforeBegin);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	int hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::resolve(Bucket& bucket, const key_type& key, const hash_code code, _VNode* prev_left, _VNode* left, _VNode* prev_right, _VNode* right)
	{
		assert(false);
		typedef typename resolution_type::Action Action;
		int act = (int)resolver(key, left->value, right->value, select, eq, *this);
		int ret = 0;
		
		if (act & (int)Action::RemoveLeft)
		{
			ret = -2;
			--elemCount;
			auto new_next = next(left);
			nalloc.destroy(left);
			nalloc.deallocate(left, 1);
			left = nullptr;
			if (prev_left)
			{
				next(prev_left) = new_next;
			}
			else
			{
				bucket = new_next;
			}
			if (new_next == right)
			{
				prev_right = prev_left;
			}
			
			act &= ~(int)Action::RemoveLeft;
		}
		
		if (act & (int)Action::RemoveRight)
		{
			--elemCount;
			auto new_next = next(right);
			nalloc.destroy(right);
			nalloc.deallocate(right, 1);
			right = nullptr;
			if (prev_right)
			{
				next(prev_right) = new_next;
			}
			else
			{
				bucket = new_next;
			}
			act &= ~(int)Action::RemoveRight;
		}
		
		if (act & (int)Action::ReinsertLeft)
		{
			auto new_next = next(left);
			if (prev_left)
			{
				next(prev_left) = new_next;
			}
			else
			{
				bucket = new_next;
			}
			
			if (left == prev_right)
			{
				prev_right = prev_left;
			}
			
			next(left) = nullptr;
			--elemCount;
			
			if (!insertNode(left))
			{
				nalloc.destroy(left);
				nalloc.deallocate(left, 1);
				left = nullptr;
				ret = -2;
			}
			else
			{
				ret = -1;
			}
		}
		
		
		if (act & (int)Action::ReinsertRight)
		{
			auto new_next = next(right);
			if (prev_right)
			{
				next(prev_right) = new_next;
			}
			else
			{
				bucket = new_next;
			}
			
			next(right) = nullptr;
			--elemCount;
			
			if (!insertNode(right))
			{
				nalloc.destroy(right);
				nalloc.deallocate(right, 1);
				right = nullptr;
			}
		}
		
		return ret;
	}
	
	
	
	
	
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::get(const Key& k) -> _VNode*
	{
		hash_code code = getCode_key(k);
		size_t index = bucketIndex(code);
		
		auto ptr = buckets[index];
		if (!ptr)
		{
			return nullptr;
		}
		while (ptr = next(ptr))
		{
			if (eq(k, select(ptr->value)))
			{
				return ptr;
			}
		}
		
		return nullptr;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::get(const Key& k) const -> const _VNode*
	{
		hash_code code = getCode_key(k);
		size_t index = bucketIndex(code);
		
		assert(index < bucketCount);
		auto ptr = buckets[index];
		if (!ptr)
		{
			return nullptr;
		}
		while (ptr = next(ptr))
		{
			if (equiv_kn(k, ptr))
			{
				return ptr;
			}
		}
		
		return nullptr;
	}
	
	/*template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::get(Key&& _k) -> _VNode*
	{
		Key k(_k);
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto ptr = buckets[index];
		if (!ptr)
		{
			return nullptr;
		}
		while (ptr = next(ptr))
		{
			if (equiv(k, ptr))
			{
				return ptr;
			}
		};
		
		return nullptr;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::get(Key&& k) const -> const _VNode*
	{
		hash_code code = hash(k);
		size_t index = bucketIndex(code);
		
		auto ptr = buckets[index];
		if (!ptr)
		{
			return nullptr;
		}
		while (ptr = next(ptr))
		{
			if (equiv(k, ptr))
			{
				return ptr;
			}
		};
		
		return nullptr;
	}*/
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	void hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::clear()
	{
		/**this = hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>();
		return;*/
		_VNode* ptr = _begin();
		_VNode* last = ptr;
		
		if (ptr != nullptr)
		{
			ptr = next(ptr);
			while (ptr)
			{
				assert(last != &beforeBegin);
				nalloc.destroy(last);
				nalloc.deallocate(last, 1);
				
				last = ptr;
				ptr = next(ptr);
			}
			assert(last != &beforeBegin);
			nalloc.destroy(last);
			nalloc.deallocate(last, 1);
		}
		
		beforeBegin.next = nullptr;
		for (int i = 0; i < bucketCount; ++i)
		{
			buckets[i] = nullptr;
		}
		elemCount = 0;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	void hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::rehash(size_type n)
	{
		if (n > bucketCount)
		{
			_rehash(n);
		}
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	void hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::reserve(size_type n)
	{
		if (n > elemCount)
		{
			auto needsRehash = _policy.needRehash(bucketCount, elemCount, n - elemCount);
			if (needsRehash.first)
			{
				_rehash(needsRehash.second);
			}
		}
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	constexpr const Policy& hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::policy() const noexcept
	{
		return _policy;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	constexpr float hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::max_load_factor() const noexcept
	{
		return policy().max_load_factor();
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	void hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::max_load_factor(float f)
	{
		_policy.max_load_factor(f);
		auto needsRehash = _policy.needRehash(bucketCount, elemCount, 0);
		if (needsRehash.first)
		{
			_rehash(needsRehash.second);
		}
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	constexpr auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::hash_function() const noexcept -> hasher
	{
		return hash;
	}
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::equiv_kn(const Key& l, const _VNode* r) const
	{
		return eq(l, select(r->value));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::equiv_key(const Key& l, const Key& r) const
	{
		return eq(l, r);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::equiv_kv(const Key& l, const T& r) const
	{
		return eq(l, select(r));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::equiv_node(const _VNode* l, const _VNode* r) const
	{
		return eq(select(l->value), select(r->value));
	}
	
	
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::getCode_value(const T& t) const -> hash_code
	{
		return hash(select(t));
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::getCode_key(const Key& k) const -> hash_code
	{
		return hash(k);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::getCode_node(const _VNode* n) const -> hash_code
	{
		return n->code;
	}
	
	
	
	// Iterator Methods
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::begin() -> iterator
	{
		return iterator(_begin());
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::end() -> iterator
	{
		return iterator(nullptr);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::begin() const -> const_iterator
	{
		return const_iterator(_begin());
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::end() const -> const_iterator
	{
		return const_iterator(nullptr);
	}
	
	
	
	
	
	
	
	
	
	
	// Iterator Method Definitions
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::iterator(_VNode* n) : node(n)
	{
		
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::iterator() : node(nullptr)
	{
		
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::iterator(const iterator& it) : node(it.node)
	{
		
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator++() -> iterator&
	{
		node = next(node);
		return *this;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator++(int) -> iterator
	{
		auto old = node;
		node = next(node);
		return iterator(old);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator*() const -> value_type&
	{
		return node->value;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator->() const -> value_type*
	{
		return &node->value;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator const_iterator() const
	{
		return const_iterator(node);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator==(const iterator& it) const
	{
		return node == it.node;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator!=(const iterator& it) const
	{
		return node != it.node;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator==(const const_iterator& it) const
	{
		return node == it.node;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::iterator::operator!=(const const_iterator& it) const
	{
		return node != it.node;
	}
	
	
	
	
	
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::const_iterator(const _VNode* n) : node(n)
	{
		
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::const_iterator() : node(nullptr)
	{
		
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::const_iterator(const const_iterator& it) : node(it.node)
	{
		
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::const_iterator(const iterator& it) : node(it.node)
	{
		
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::operator++() -> const_iterator&
	{
		node = next(node);
		return *this;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::operator++(int) -> const_iterator
	{
		auto old = node;
		node = next(node);
		return iterator(old);
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	auto hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::operator*() const -> const value_type&
	{
		return node->value;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::operator==(const const_iterator& it) const
	{
		return node == it.node;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::operator!=(const const_iterator& it) const
	{
		return node != it.node;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::operator==(const iterator& it) const
	{
		return node == it.node;
	}
	
	template <class Key, class T, class Selector, class Hash, class Equals, class Alloc, class Policy, template <class...> class Resolution>
	bool hash_table<Key, T, Selector, Hash, Equals, Alloc, Policy, Resolution>::const_iterator::operator!=(const iterator& it) const
	{
		return node != it.node;
	}
	
}

#endif