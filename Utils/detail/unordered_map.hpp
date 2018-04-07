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
	
	
	
	
	// Member Functions
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::empty() const
	{
		return _table.size() != 0;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::size() const -> size_type
	{
		return _table.size();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::get_allocator() const -> allocator_type
	{
		return _table.get_allocator();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(const key_type& k) -> mapped_type&
	{
		return _table.get(k)->value.second;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(key_type&& k) -> mapped_type&
	{
		return _table.get(forward<key_type&&>(k))->value.second;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(const key_type& k) const -> const mapped_type&
	{
		return _table.get(k)->value.second;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::at(key_type&& k) const -> const mapped_type&
	{
		return _table.get(forward<key_type&&>(k))->value.second;
	}
	
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::count(const key_type& k) const -> size_type
	{
		return _table.get(k) != nullptr;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::count(key_type&& k) const -> size_type
	{
		return _table.get(forward<key_type&&>(k)) != nullptr;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	void unordered_map<Key, T, Hash, KeyEqual, Alloc>::clear()
	{
		_table.clear();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	float unordered_map<Key, T, Hash, KeyEqual, Alloc>::max_load_factor() const
	{
		return _table.max_load_factor();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	void unordered_map<Key, T, Hash, KeyEqual, Alloc>::max_load_factor(float f)
	{
		_table.max_load_factor(f);
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	float unordered_map<Key, T, Hash, KeyEqual, Alloc>::load_factor() const
	{
		return ((long double)_table.size())/_table.bucketsSize();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::hash_function() const -> hasher
	{
		return _table.hash_function();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::key_eq() const -> key_equal
	{
		return _table.key_eq();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::bucket_count() const -> size_type
	{
		return _table.bucketsSize();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::max_bucket_count() const -> size_type
	{
		return _table.max_bucketsSize();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::bucket_size(size_type n) const -> size_type
	{
		return _table.size_at(n);
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	void unordered_map<Key, T, Hash, KeyEqual, Alloc>::rehash(size_type n)
	{
		_table.rehash(n);
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	void unordered_map<Key, T, Hash, KeyEqual, Alloc>::reserve(size_type n)
	{
		_table.reserve(n);
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
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::operator=(const unordered_map& t) -> unordered_map&
	{
		_table = t._table;
		return *this;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::operator=(unordered_map&& t) -> unordered_map&
	{
		_table = move(t._table);
		return *this;
	}
	
	
	
	
	
	
	
	
	
	// Iterator Methods
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::begin() -> iterator
	{
		return iterator(_table.begin());
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::end() -> iterator
	{
		return iterator(_table.end());
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::begin() const -> const_iterator
	{
		return const_iterator(_table.begin());
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::end() const -> const_iterator
	{
		return const_iterator(_table.end());
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::cbegin() const -> const_iterator
	{
		return const_iterator(_table.begin());
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::cend() const -> const_iterator
	{
		return const_iterator(_table.end());
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// Iterator Method Definitions
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator++() -> iterator&
	{
		++_it;
		return *this;
	}
			
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator++(int) -> iterator
	{
		auto old = *this;
		this->operator++();
		return old;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator*() const -> value_type&
	{
		return *_it;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator->() const -> value_type*
	{
		return _it.operator->();
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator const_iterator() const
	{
		return const_iterator(*this);
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator==(const iterator& it) const
	 {
	 	return _it == it._it;
	 }
	 
	 template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator!=(const iterator& it) const
	 {
	 	return _it != it._it;
	 }
	 
	 template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator==(const const_iterator& it) const
	 {
	 	return _it == it._it;
	 }
	 
	 template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::iterator::operator!=(const const_iterator& it) const
	 {
	 	return _it != it._it;
	 }
	 
	 
	 
	 
	 
	 template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::const_iterator::operator++() -> const_iterator&
	{
		++_it;
		return *this;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::const_iterator::operator++(int) -> const_iterator
	{
		auto old = *this;
		this->operator++();
		return old;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 auto unordered_map<Key, T, Hash, KeyEqual, Alloc>::const_iterator::operator*() -> const value_type&
	{
		return *_it;
	}
	
	template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::const_iterator::operator==(const const_iterator& it) const
	 {
	 	return _it == it._it;
	 }
	 
	 template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::const_iterator::operator!=(const const_iterator& it) const
	 {
	 	return _it != it._it;
	 }
	 
	 template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::const_iterator::operator==(const iterator& it) const
	 {
	 	return _it == it._it;
	 }
	 
	 template <class Key, class T, class Hash, class KeyEqual, class Alloc>
	 bool unordered_map<Key, T, Hash, KeyEqual, Alloc>::const_iterator::operator!=(const iterator& it) const
	 {
	 	return _it != it._it;
	 }
}

#endif