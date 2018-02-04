#include "int_to_str.h"
#include <Common.h>

int c_int_to_str(int n, char* cstr, unsigned int max_len)
{
    if (max_len < 2)
    {
        return 0;
    }
    unsigned int base = 10;
    if (n == 0)
    {
        cstr[0] = '0';
        cstr[1] = 0;
        return 1;
    }

    if (n < 0)
    {
        cstr[0] = '-';
        return c_int_to_str(-1*n, cstr+1, max_len-1);
    }




    for (base = 10; n / base != 0; base *= 10)
    {}

    base /= 10;

    int i = 0;
    uint32_t tmp;
    while (base > 0)
    {
        tmp = n / base;
        cstr[i] = (unsigned char)48 + tmp;
        n -= tmp * base;
        base /= 10;
        ++i;
        if (i >= max_len)
        {
            return 0;
        }
    }
    if (i >= max_len)
    {
        return 0;
    }
    cstr[i] = 0;
    return 1;
}