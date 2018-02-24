#ifndef INCLUDED_STRING_HH
#define INCLUDED_STRING_HH

#include <kernel/utils/Allocator.hh>
#include <kernel/utils/Char_Traits.hh>
#include <stddef.h>

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
        typedef __gnu_cxx::__normal_iterator<pointer, basic_string>         iterator; 
        typedef __gnu_cxx::__normal_iterator<const_pointer, basic_string>   const_iterator;



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

            static constexpr size_type  _S_max_size;
            static constexpr Char_t     _S_terminal;

            static size_t _S_empty_rep_storage[];

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

                        if (__gnu_cxx::__exchange_and_add_dispatch(&this->_M_refcount, -1) <= 0)
                        {
                            _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&this->_M_refcount);
                            _M_destroy(__a);
                        }
                    }
            }

            void _M_destroy(const Alloc&);

            Char_t* _M_refcopy()
            {
                #ifndef _GLIBCXX_FULLY_DYNAMIC_STRING
                    if (__builtin_expect(this != &_S_empty_rep(), false))
                #endif
                    {
                        __gnu_cxx::__atomic_add_dispatch(&this->_M_refcount, 1);
                        return _M_refdata();
                    }
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
            return _M_dataplus._M_p;
        }

        Char_t* _M_data(Char_t* __p)
        {
            return (_M_dataplus._M_p = __p);
        }

        _Rep* _M_rep() const
        {
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
                __throw_out_of_range(__N(__s));
            }
            return __pos;
        }

        void _M_check_length(size_type __n1, size_type __n2, const char* __s) const
        {
            if (this->max_size() - (this->size() - __n1) < __n2)
            {
                __throw_length_error(__N(__s));
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

            if (__d > __gnu_cxx::__numeric_traits<int>::__max)
            {
                return __gnu_cxx::__numeric_traits<int>::__max;
            }
            else if (__d < __gnu_cxx::__numeric_traits<int>::__min)
            {
                return __gnu_cxx::__numeric_traits<int>::__min;
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
        basic_string();
        explicit basic_string(const Alloc& alloc);

        //Copy Constructors
        basic_string(const basic_string&);
        basic_string(const basic_string&, const Alloc&);

        //Substring Constructor
        basic_string(const basic_string&, size_t pos, size_t len = npos, const Alloc& = Alloc());

        //CString Constructor
        basic_string(const Char_t*, const Alloc& = Alloc());

        basic_string(const Char_t*, size_t n, const Alloc& = Alloc());

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
        
    };


    typedef basic_string<char, Char_Traits<char>, Allocator<char>> string;

}
#endif