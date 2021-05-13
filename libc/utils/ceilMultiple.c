#include <utils.h>

uint32_t ceilMultiple(uint32_t num, uint32_t multiple)
{
    return ((num % multiple != 0) ? (multiple + num) - (num % multiple) : num);
}