#ifndef INCLUDED_STRING_HH
#define INCLUDED_STRING_HH

#include <Utils/Allocator.hh>
#include <Utils/Char_Traits.hh>
#include <stddef.h>
#include "limits"
#include "Iterator"
#include "Compare.hh"
#include "detail/functexcept.hh"
#include "Atomicity.hh"
#include "detail/c++config.hh"
#include "hash"
#include "StringFwd.hh"

namespace Utils
{
    


    template <class Char_t, class CharTraits, class Alloc>
    class basic_string
    {
        private:
        typedef typename Alloc::template rebind<Char_t>::other _CharT_alloc_type;


        public:
        typedef CharTraits                                                  traits_type;
        typedef typename CharTraits::char_type                              value_type;
        typedef Alloc                                                       allocator_type;
        typedef typename _CharT_alloc_type::size_type                       size_type;
        typedef typename _CharT_alloc_type::difference_type                 difference_type;
        typedef typename _CharT_alloc_type::reference                       reference;
        typedef typename _CharT_alloc_type::const_reference                 const_reference;
        typedef typename _CharT_alloc_type::pointer                         pointer;
        typedef typename _CharT_alloc_type::const_pointer                   const_pointer;
        typedef normal_iterator<pointer, basic_string>         iterator; 
        typedef normal_iterator<const_pointer, basic_string>   const_iterator;



        constexpr static size_type npos = static_cast<size_type>(-1);

        private:

        struct _Rep_base
        {
            size_type       _M_length;
            size_type       _M_capacity;
            int    _M_refcount;
        };

        struct _Rep : _Rep_base
        {
            typedef typename Alloc::template rebind<char>::other _Raw_bytes_alloc;

            static constexpr size_type  _S_max_size = (((npos - sizeof(_Rep_base))/sizeof(Char_t)) - 1) / 4;
            static constexpr Char_t     _S_terminal = Char_t();

            //static size_type _S_empty_rep_storage[];
            typedef struct
            {
            	_Rep_base base;
            	char term;
            } empty_rep_t;
            
            static empty_rep_t _S_empty_rep_storage;

            static _Rep& _S_empty_rep()
            {
                void* __p = reinterpret_cast<void*>(&_S_empty_rep_storage);
                return *reinterpret_cast<_Rep*>(__p);
            }

            bool _M_is_leaked() const
            {
                return this->_M_refcount < 0;
            }

            bool _M_is_shared() const
            {
                return this->_M_refcount > 0;
            }

            void  _M_set_leaked()
            {
                this->_M_refcount = -1;
            }

            void _M_set_sharable()
            {
                this->_M_refcount = 0;
            }

            void _M_set_length_and_sharable(size_type __n)
            {
                #ifndef _GLIBCXX_FULLY_DYNAMIC_STRING
                    if (__builtin_expect(this != &_S_empty_rep(), false))
                #endif
                    {
                        this->_M_set_sharable();
                        this->_M_length = __n;
                        traits_type::assign(this->_M_refdata()[__n], _S_terminal);
                    }
            }

            Char_t* _M_refdata() throw()
            {
            	assert(this + 1);
                return reinterpret_cast<Char_t*>(this + 1);
            }

            Char_t* _M_grab(const Alloc& __alloc1, const Alloc& __alloc2)
            {
                return (!_M_is_leaked() && __alloc1 == __alloc2) ? _M_refcopy() : _M_clone(__alloc1);
            }

            //Create and Destroy
            
            static _Rep* _S_create(size_type, size_type, const Alloc&);

            void _M_dispose(const Alloc& __a)
            {
                #ifndef _GLIBCXX_FULLY_DYNAMIC_STRING
                    if (__builtin_expect(this != &_S_empty_rep(), false))
                #endif
                    {
                        // // Be race-detector-friendly.  For more info see bits/c++config.
                        // _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(&this->_M_refcount);

                        if (/*__gnu_cxx::*/__exchange_and_add_dispatch(&this->_M_refcount, -1) <= 0)
                        {
                           _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&this->_M_refcount);
                            _M_destroy(__a);
                        }
                    }
            }

            void _M_destroy(const Alloc& a)
            {
            	const size_type size = sizeof(_Rep_base) + (this->_M_capacity + 1) * sizeof(Char_t);
            	_Raw_bytes_alloc(a).deallocate(reinterpret_cast<char*>(this), size);
            }

            Char_t* _M_refcopy()
            {
                #ifndef _GLIBCXX_FULLY_DYNAMIC_STRING
                    if (__builtin_expect(this != &_S_empty_rep(), false))
                #endif
                    {
                        /*__gnu_cxx::*/__atomic_add_dispatch(&this->_M_refcount, 1);
                        
                    }
                    return _M_refdata();
            }

            Char_t* _M_clone(const Alloc&, size_type __res = 0);
        };

        // Use empty-base optimization: http://www.cantrip.org/emptyopt.html
        struct _Alloc_hider : Alloc
        {
            _Alloc_hider(Char_t* __dat, const Alloc& __a) : Alloc(__a), _M_p(__dat)
            {}

            Char_t* _M_p; //The actual data
        };
        

        private:
        mutable _Alloc_hider _M_dataplus;

        Char_t* _M_data() const
        {
        	assert(_M_dataplus._M_p);
            return _M_dataplus._M_p;
        }

        Char_t* _M_data(Char_t* __p)
        {
        	assert(__p);
            return (_M_dataplus._M_p = __p);
        }

        _Rep* _M_rep() const
        {
        	assert(_M_data());
            return &((reinterpret_cast<_Rep*>(_M_data()))[-1]);
        }

        iterator _M_ibegin() const
        {
            return iterator(_M_data());
        }

        iterator _M_iend() const
        {

            return iterator(_M_data() + this->size());
        }

        void _M_leak()
        {
            if (!_M_rep()->_M_is_leaked())
            {
                _M_leak_hard();
            }
        }

        size_type _M_check(size_type __pos, const char* __s) const
        {
            if (__pos > this->size())
            {
                __throw_out_of_range(__s);
            }
            return __pos;
        }

        void _M_check_length(size_type __n1, size_type __n2, const char* __s) const
        {
            if (this->max_size() - (this->size() - __n1) < __n2)
            {
                __throw_length_error(__s);
            }
        }

        size_type _M_limit(size_type __pos, size_type __off) const
        {
            const bool __testoff = __off < this->size() - __pos;
            return __testoff ? __off : this->size() - __pos;
        }

        bool _M_disjunct(const Char_t* __s) const
        {
            return (less<const Char_t*>()(__s, _M_data()) || less<const Char_t*>()(_M_data() + this->size(), __s));
        }

        static void _M_copy(Char_t* __d, const Char_t* __s, size_type __n)
        {
            if (__n == 1)
            {
                traits_type::assign(*__d, *__s);
            }
            else
            {
                traits_type::copy(__d, __s, __n);
            }
        }

        static void _M_move(Char_t* __d, const Char_t* __s, size_type __n)
        {
            if (__n == 1)
            {
                traits_type::assign(*__d, *__s);
            }
            else
            {
                traits_type::move(__d, __s, __n);
            }
        }

        static void _M_assign(Char_t* __d, size_type __n, Char_t __c)
        {
            if (__n == 1)
            {
                traits_type::assign(*__d, __c);
            }
            else
            {
                traits_type::assign(__d, __n, __c);
            }
        }

        template <class _Iter>
        static void _S_copy_chars(Char_t* __p, _Iter __k1, _Iter __k2)
        {
            for (; __k1 != __k2; ++__k1, ++__p)
            {
                traits_type::assign(*__p, *__k1);
            }
        }

         static void _S_copy_chars(Char_t* __p, iterator __k1, iterator __k2)
        {
            _S_copy_chars(__p, __k1.base(), __k2.base());
        }

        static void _S_copy_chars(Char_t* __p, const_iterator __k1, const_iterator __k2)
        {
            _S_copy_chars(__p, __k1.base(), __k2.base());
        }

        static void _S_copy_chars(Char_t* __p, Char_t* __k1, Char_t* __k2)
        {
            _S_copy_chars(__p, __k1, __k2 - __k1);
        }

        static void _S_copy_chars(Char_t* __p, const Char_t* __k1, const Char_t* __k2)
        {
            _S_copy_chars(__p, __k1, __k2 - __k1);
        }

        static int _S_compare(size_type __n1, size_type __n2)
        {
            const difference_type __d = difference_type(__n1 - __n2);

            if (__d > numeric_limits<int>::max())
            {
                return numeric_limits<int>::max();
            }
            else if (__d < numeric_limits<int>::min())
            {
                return numeric_limits<int>::min();
            }
            else
            {
                return int(__d);
            }
        }

        void _M_mutate(size_type __pos, size_type __len1, size_type __len2);

        void _M_leak_hard();

        static _Rep& _S_empty_rep()
        {
            return _Rep::_S_empty_rep();
        }

        public:



        

        //Default Constructors
        basic_string()
        #ifndef _GLIBCXX_FULLY_DYNAMIC_STRING
         : _M_dataplus(_S_empty_rep()._M_refdata(), Alloc())
        #else
         : _M_dataplus(_S_construct(size_type(), Char_t(), Alloc()), Alloc())
        #endif
        {
        	assert(_M_data());
        }
        
        explicit basic_string(const Alloc& alloc);

        //Copy Constructors
        basic_string(const basic_string&);
        basic_string(const basic_string&, const Alloc&);

        //Substring Constructor
        basic_string(const basic_string&, size_t pos, size_t len = npos, const Alloc& = Alloc());

        //CString Constructor
        basic_string(const Char_t*, const Alloc&);
        basic_string(const Char_t*);

        basic_string(const Char_t*, size_t n, const Alloc&);
        basic_string(const Char_t*, size_t n);

        //Fill Constructor
        basic_string(size_t n, Char_t c, const Alloc& = Alloc());

        //Range Constructor
        template <class InputIter>
        basic_string(InputIter first, InputIter last, const Alloc& = Alloc());

        // //Initializer List Constructor
        // basic_string(std::initializer_list<Char_t> il, const Alloc& = Alloc());
        
        //Move Constructors
        basic_string(basic_string&& str) noexcept;
        basic_string(basic_string&& str, const Alloc&);


        //Destructor
        ~basic_string()
        {
            _M_rep()->_M_dispose(this->get_allocator());
        }


        //Operators
        basic_string& operator=(const basic_string& s)
        {
        	return this->assign(s);
        }
        
        basic_string& operator=(const Char_t* s)
        {
        	return this->assign(basic_string(s, get_allocator()));
        }
        
        
        inline basic_string& operator+=(const basic_string& _rhs)
        {
        	return this->append(_rhs);
        }
        
        inline basic_string& operator+=(const Char_t* _rhs)
        {
        	return this->append(_rhs);
        }
        
        inline basic_string& operator+=(const Char_t _rhs)
        {
        	return this->append(1, _rhs);
        }
        
        value_type& operator[](size_t pos);
        const value_type& operator[](size_t pos) const;
        
        
        
        
        
        
        
        
        
        //Members
        size_type size() const noexcept
        {
        	assert(_M_rep());
        	return _M_rep()->_M_length;
        }
        
        __attribute__((always_inline))
        size_type length() const noexcept
        {
        	return size();
        }
        
        __attribute__((always_inline))
        bool empty() const noexcept
        {
        	return size() == 0;
        }
        
        const Char_t* c_str() const
        {
        	return _M_data();
        }
        
        basic_string& assign(const basic_string& s);
        
        basic_string& assign(basic_string&& s)
        {
        	this->swap(s);
        	return *this;
        }
        
        basic_string& erase(size_type index = 0, size_type count = npos);
        
        basic_string& append(const basic_string&);
        basic_string& append(const basic_string&, size_type pos, size_type num);
        basic_string& append(const Char_t*, size_type len);
        basic_string& append(size_type n, Char_t c);
        basic_string& append(const Char_t* s)
        {
        	return this->append(s, traits_type::length(s));
        }
        
        template <class It>
        basic_string& append(It _beg, It _end)
        {
        	return this->replace(_M_iend(), _M_iend(), _beg, _end);
        }
        
        value_type& at(size_t pos);
        const value_type& at(size_t pos) const;
        basic_string substr(size_t pos = 0, size_t len = npos) const;
        
        
        
        
        public:
        
        allocator_type get_allocator() const
        {
        	return _M_dataplus;
        }
        
        
        
        int compare(const basic_string& _str) const
        {
        	const size_type _s = size();
        	const size_type _s2 = _str.size();
        	
        	const size_type _len = (_s < _s2 ? _s : _s2);
        	int _r = traits_type::compare(_M_data(), _str._M_data(), _len);
        	
        	if (!_r)
        	{
        		_r = _S_compare(_s, _s2);
        	}
        	return _r;
        }
        
        size_type max_size() const
        {
        	return _Rep::_S_max_size;
        	
        }
		
		size_type capacity() const
		{
			assert(_M_rep());
			return _M_rep()->_M_capacity;
		}
		
		
        void reserve(size_type = 0);
        void swap(basic_string&);
        
        size_type find(const basic_string&, size_type pos = 0) const;
        size_type find(const value_type*, size_type pos, size_type count) const;
        size_type find(const value_type*, size_type pos = 0) const;
        size_type find(value_type, size_type pos = 0) const;
        template <class T>
        size_type find(const T&, size_type pos = 0) const;
        
        private:
        template <class InIt>
        static Char_t* _S_construct(InIt beg, InIt end, const Alloc& a);//, input_iterator_tag);
        
    };
    
    
    //template <class C, class T, class A>
    //typename basic_string<C, T, A>::size_type basic_string<C, T, A>::_Rep::_S_empty_rep_storage[] = { /*'\0'*/0, 0, 0 };
    
    template <class C, class T, class A>
    typename basic_string<C, T, A>::_Rep::empty_rep_t basic_string<C, T, A>::_Rep::_S_empty_rep_storage = { { 0, 0, 0 }, '\0' };
    
    
    template <class Char_t, class T, class Alloc>
    inline bool operator==(const basic_string<Char_t, T, Alloc>& _lhs, const basic_string<Char_t, T, Alloc>& _rhs)
    {
    	return _lhs.compare(_rhs) == 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator!=(const basic_string<Char_t, T, Alloc>& _lhs, const basic_string<Char_t, T, Alloc>& _rhs)
    {
    	return _lhs.compare(_rhs) != 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator==(const basic_string<Char_t, T, Alloc>& _lhs, const Char_t* _rhs)
    {
    	return _lhs.compare(_rhs) == 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator!=(const basic_string<Char_t, T, Alloc>& _lhs, const Char_t* _rhs)
    {
    	return _lhs.compare(_rhs) != 0;
    }
    
    
    
    
    template <class Char_t, class T, class Alloc>
    inline basic_string<Char_t, T, Alloc> operator+(const basic_string<Char_t, T, Alloc>& _lhs, const basic_string<Char_t, T, Alloc>& _rhs)
    {
    	basic_string<Char_t, T, Alloc> res = _lhs;
    	res += _rhs;
    	return res;
    }
    
    
    template <class Char_t, class T, class Alloc>
    inline basic_string<Char_t, T, Alloc> operator+(const basic_string<Char_t, T, Alloc>& _lhs, const Char_t* _rhs)
    {
    	basic_string<Char_t, T, Alloc> res = _lhs;
    	res += _rhs;
    	return res;
    }
    
    
    template <class Char_t, class T, class Alloc>
    inline basic_string<Char_t, T, Alloc> operator+(const Char_t* _lhs, const basic_string<Char_t, T, Alloc>& _rhs)
    {
    	basic_string<Char_t, T, Alloc> res = _lhs;
    	res += _rhs;
    	return res;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator>(const basic_string<Char_t, T, Alloc>& lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return lhs.compare(rhs) > 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator>(const basic_string<Char_t, T, Alloc>& lhs, const Char_t* rhs)
    {
    	return lhs.compare(rhs) > 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator>(const Char_t* lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return rhs.compare(lhs) < 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator>=(const basic_string<Char_t, T, Alloc>& lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return lhs.compare(rhs) >= 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator>=(const basic_string<Char_t, T, Alloc>& lhs, const Char_t* rhs)
    {
    	return lhs.compare(rhs) >= 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator>=(const Char_t* lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return rhs.compare(lhs) <= 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator<(const basic_string<Char_t, T, Alloc>& lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return lhs.compare(rhs) < 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator<(const basic_string<Char_t, T, Alloc>& lhs, const Char_t* rhs)
    {
    	return lhs.compare(rhs) < 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator<(const Char_t* lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return rhs.compare(lhs) > 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator<=(const basic_string<Char_t, T, Alloc>& lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return lhs.compare(rhs) <= 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator<=(const basic_string<Char_t, T, Alloc>& lhs, const Char_t* rhs)
    {
    	return lhs.compare(rhs) <= 0;
    }
    
    template <class Char_t, class T, class Alloc>
    inline bool operator<=(const Char_t* lhs, const basic_string<Char_t, T, Alloc>& rhs)
    {
    	return rhs.compare(lhs) >= 0;
    }


    typedef basic_string<char, Char_Traits<char>, allocator<char>> string;
    
    
    template <class>
    struct hash;
    
    template <class Char_t, class Traits, class Alloc>
    struct hash<basic_string<Char_t, Traits, Alloc>> : public hash_base<size_t, basic_string<Char_t, Traits, Alloc>>
    {
    	private:
    	typedef hash_base<size_t, basic_string<Char_t, Traits, Alloc>> _Base;
    	
    	public:
    	typedef typename _Base::result_type result_type;
    	typedef typename _Base::argument_type argument_type;
    	
    	result_type operator()(const argument_type& arg) const
    	{
    		return detail::a_hash_func<result_type>::hash(arg.c_str(), arg.length()*sizeof (typename argument_type::value_type));
    	}
    };
    #define __UTILS_STRING_DEFINED__
    
    string to_string(int);
    string to_string(long);
    string to_string(long long);
    string to_string(unsigned);
    string to_string(unsigned long);
    string to_string(unsigned long long);
    string to_string(float);
    string to_string(double);
    string to_string(long double);
    
    
    template <class T, class CharT>
    class basic_ostream;
    
    template <class T, class CharT>
    class basic_istream;
    
    
    template <class T, class Traits, class Alloc>
    basic_ostream<T, Traits>& operator<<(basic_ostream<T, Traits>&, const basic_string<T, Traits, Alloc>& str);
    
    template <class T, class Traits, class Alloc>
    basic_istream<T, Traits>& operator>>(basic_istream<T, Traits>&, basic_string<T, Traits, Alloc>& str);
    
    #ifdef __UTILS_OSTREAM_DEFINED__
    #ifndef __UTILS_OSTREAM_STRING_DEFINED__
    #define __UTILS_OSTREAM_STRING_DEFINED__
    
    template <class T, class Traits, class Alloc>
    basic_ostream<T, Traits>& operator<<(basic_ostream<T, Traits>& os, const basic_string<T, Traits, Alloc>& str)
    {
    	return (os << str.c_str());
    }
    
    #endif
    #endif

}
#endif