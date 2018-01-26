#ifndef INCLUDED_STRING_HH
#define INCLUDED_STRING_HH

#include <kernel/utils/Allocator.hh>
#include <kernel/utils/Char_Traits.hh>
#include <stddef.h>

namespace Utils
{
    template <class Char_t, class CharTraits, class Alloc>
    class Basic_String
    {
        private:
        typedef typename Alloc::template rebind<Char_t>::other _CharT_alloc_type;


        public:
        typedef CharTraits                                  traits_type;
        typedef typename CharTraits::char_type              value_type;
        typedef Alloc                                       allocator_type;
        typedef typename _CharT_alloc_type::size_type       size_type;
        typedef typename _CharT_alloc_type::difference_type difference_type;
        typedef typename _CharT_alloc_type::reference       reference;
        typedef typename _CharT_alloc_type::const_reference const_reference;
        typedef typename _CharT_alloc_type::pointer         pointer;
        typedef typename _CharT_alloc_type::const_pointer   const_pointer;




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

            void _M_destroy(const Alloc&) throw;

            Char_t* _M_refcopy() throw()
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
        struct _Alloc_hider : _Alloc_hider
        {
            _Alloc_hider(Char_t* __dat, const Alloc& __a) : _Alloc(__a), _M_p(__dat)
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
                __throw_length_error(__N(__ss));
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

        static void _M_copy(Char_t* __d, const Char_t* __s, size_type n)
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

        static void _M_move(Char_t* __d, const Char_t* __s, size_type n)
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

        protected:

        public:



        

        //Default Constructors
        Basic_String();
        explicit Basic_String(const Alloc& alloc);

        //Copy Constructors
        Basic_String(const Basic_String&);
        Basic_String(const Basic_String&, const Alloc&);

        //Substring Constructor
        Basic_String(const Basic_String&, size_t pos, size_t len = npos, const Alloc& = Alloc());

        //CString Constructor
        Basic_String(const Char_t*, const Alloc& = Alloc());

        Basic_String(const Char_t*, size_t n, const Alloc& = Alloc());

        //Fill Constructor
        Basic_String(size_t n, Char_t c, const Alloc& = Alloc());

        //Range Constructor
        template <class InputIter>
        Basic_String(InputIter first, InputIter last, const Alloc& = Alloc());

        // //Initializer List Constructor
        // Basic_String(std::initializer_list<Char_t> il, const Alloc& = Alloc());
        
        //Move Constructors
        Basic_String(Basic_String&& str) noexcept;
        Basic_String(Basic_String&& str, const Alloc&);



    };


    typedef Basic_String<char, Char_Traits<char>, Allocator<char>> String;

}
#endif