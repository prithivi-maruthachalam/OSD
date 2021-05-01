#include <coolMath.h>

uint32_t ceil_pow2(uint32_t num)
{
    num = (num > 0) ? num - 1 : 0;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    return (++num);
}