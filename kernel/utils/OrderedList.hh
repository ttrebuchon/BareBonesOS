#ifndef INCLUDED_ORDEREDLIST_HH
#define INCLUDED_ORDEREDLIST_HH

#include "List.hh"
#include "Compare.hh"

namespace Utils {

    template <class T, class Comp = Less<T>, class Alloc = Allocator<T>>
    class OrderedList : public List<T, Alloc>
    {
        private:

        protected:

        public:
    };
}
#endif