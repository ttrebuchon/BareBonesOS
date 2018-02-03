#ifndef INCLUDED_ORDEREDARRAY_HH
#define INCLUDED_ORDEREDARRAY_HH

#include <Common.h>
#include "Compare.hh"

namespace Utils
{
    template <class T, class Compare = Less<T>>
    class OrderedArray
    {
        private:

        protected:
        T* array;
        size_t _size;
        Compare comp;

        public:
        const size_t Capacity;

        OrderedArray(void* ptr, const size_t cap) : array((T*)ptr), _size(0), comp(), Capacity(cap)
        {}

        size_t size() const
        {
            return _size;
        }

        inline const T& operator[](const size_t n) const
        {
            return array[n];
        }

        inline const T& at(const size_t n) const
        {
            return array[n];
        }

        bool insert(const T item)
        {
            if (_size == Capacity)
            {
                return false;
            }
            size_t i;
            for (i = 0; i < _size; ++i)
            {
                if (!comp(item, array[i]))
                {
                    break;
                }
            }
            if (i == _size)
            {
                array[i] = item;
            }
            else
            {
                T tmp = item;
                for (; i <= _size; ++i)
                {
                    T tmp2 = array[i];
                    array[i] = tmp;
                    tmp = tmp2;
                }
            }
            ++_size;
            return true;
        }

        bool erase(const size_t index)
        {
            for (size_t i = index; i < _size - 1; ++i)
            {
                array[i] = array[i+1];
            }
            --_size;
            return true;
        }
    };
}

#endif