#ifndef INCLUDED_STRICTORDEREDARRAY_HH
#define INCLUDED_STRICTORDEREDARRAY_HH

#include <Common.h>
#include "Compare.hh"

namespace Utils
{
    template <size_t N, class T, class Compare = Less<T>>
    class StrictOrderedArray
    {
        private:

        protected:
        T array[N];
        size_t _size;
        Compare comp;

        public:
        constexpr static size_t Capacity = N;

        StrictOrderedArray();

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

        bool erase(const size_t index);
    };
}

#endif