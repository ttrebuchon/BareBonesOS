#ifndef INCLUDED_HASH_TABLE_HH
#define INCLUDED_HASH_TABLE_HH

#include <Common.h>
#include "hash_table_policy.hh"
#include <Utils/Compare.hh>
#include <Utils/Allocator.hh>
#include <Utils/Allocator_Traits.hh>
#include <Utils/hash>

namespace Utils
{
	namespace detail
	{
		struct hash_node_base
		{
			hash_node_base* next;
			
		};
		
		template <class T>
		struct hash_node : public hash_node_base
		{
			T value;
			size_t code;
		};
	}
	
	template <class Key, class T, class Selector, class Hash = hash<Key>, class Equals = equal_to<T>, class Alloc = Allocator<T>, class Policy = detail::hash_table_policy>
	class hash_table : protected Policy
	{
		public:
		typedef Key key_type;
		typedef T value_type;
		typedef Hash hasher;
		typedef Equals equals_type;
		typedef Alloc allocator_type;
		
		
		protected:
		typedef detail::hash_node<T>* Bucket;
		typedef detail::hash_node<T> _VNode;
		typedef const _VNode c_VNode;
		typedef decltype(declval<hasher>()(declval<key_type>())) hash_code;
		
		typedef typename allocator_type::template rebind<_VNode>::other node_allocator_type;
		
		Selector select;
		hasher hash;
		equals_type eq;
		mutable allocator_type alloc;
		mutable node_allocator_type nalloc;
		Policy policy;
		
		Bucket* buckets;
		detail::hash_node_base* pre;
		size_t bucketCount;
		size_t elemCount;
		
		detail::hash_node_base beforeBegin;
		
		size_t bucketIndex(const T& t) const;
		size_t bucketIndex(const Key& k) const;
		size_t bucketIndex(const hash_code h) const;
		size_t bucketIndex(const _VNode*) const;
		
		_VNode* findNode(const size_t bucketIndex, const Key&, const hash_code);
		const _VNode* findNode(const size_t bucketIndex, const Key&, const hash_code) const;
		detail::hash_node_base* findBeforeNode(const size_t bucketIndex, const Key&, const hash_code);
		_VNode* allocateNode(const Key&) const;
		_VNode* allocateNode(const T&) const;
		void rehash(size_t bCount);
		_VNode* _begin();
		const _VNode* _begin() const;
		
		static _VNode*& next(detail::hash_node_base*);
		static const _VNode* const& next(const detail::hash_node_base*);
		
		public:
		class iterator
		{
			
		};
		
		class const_iterator
		{
			
		};
		
		
		hash_table(size_t bucketHint, const Selector&, const hasher& hash, const equals_type&, const allocator_type&);
		hash_table(const hash_table&);
		~hash_table();
		
		detail::hash_node<T>* get(const Key&);
		detail::hash_node<T>* get(const T&);
		detail::hash_node<T>* getCreate(const Key&);
		detail::hash_node<T>* getCreate(const T&);
		detail::hash_node<T>* getCreate(Key&&);
		inline size_t size() const
		{
			return elemCount;
		}
		
		bool equiv(const Key&, const _VNode*) const;
		bool equiv(const Key&, const Key&) const;
		bool equiv(const Key&, const T&) const;
		inline bool equiv(const T& l, const _VNode* r) const
		{
			return equiv(select(l), r);
		}
		inline bool equiv(const T& l, const Key& r) const
		{
			return equiv(select(l), r);
		}
		inline bool equiv(const T& l, const T& r) const
		{
			return equiv(select(l), r);
		}
		bool equiv(const _VNode*, const _VNode*) const;
		
		
		inline hash_code getCode(const T&) const;
		inline hash_code getCode(const Key&) const;
		inline hash_code getCode(const _VNode*) const;
		
		
	};
	
}

#endif