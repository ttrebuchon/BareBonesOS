#ifndef INCLUDED_UNORDERED_MAP_HPP
#define INCLUDED_UNORDERED_MAP_HPP

namespace Utils
{
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	unordered_map<Key, T, Hash, KeyEqual, Alloc>::unordered_map() : unordered_map(10)
	{
		
	}
	
	/*template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	unordered_map<Key, T, Hash, KeyEqual, Alloc>::unordered_map(size_t bucketCount, const hasher& hash, const key_equal& keyEq, const allocator_type& alloc) : _table(bucketCount, value_type_hasher(hash), value_type_equal(keyEq), alloc)
	{
		
	}*/
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	unordered_map<Key, T, Hash, KeyEqual, Alloc>::unordered_map(size_t bucketCount, const hasher& hash, const key_equal& keyEq, const allocator_type& alloc) : _table(bucketCount, Select1st(), hash, keyEq, alloc)
	{
		
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	unordered_map<Key, T, Hash, KeyEqual, Alloc>::unordered_map(const unordered_map& m) : _table(m._table)
	{
		
	}
	
	
	
	
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::size() const -> size_type
	{
		return _table.size();
	}
	
	
	
	// Operators
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	T& unordered_map<Key, T, Hash, KeyEqual, Alloc>::operator[](const key_type& k)
	{
		return _table.getCreate(forward<const key_type&>(k))->value.second;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	T& unordered_map<Key, T, Hash, KeyEqual, Alloc>::operator[](key_type&& k)
	{
		return _table.getCreate(forward<key_type&&>(k))->value.second;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	T& unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(const key_type& k)
	{
		
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	T& unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(key_type&& k)
	{
		
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	const T& unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(const key_type& k) const
	{
		
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	const T& unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(key_type&& k) const
	{
		
	}
	
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::count(const key_type& k) const -> size_type
	{
		
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::count(key_type&& k) const -> size_type
	{
		
	}
}

#endif