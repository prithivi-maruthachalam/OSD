/*fill n bytes with a value*/

#include <string.h>

void *memset(void *buf, int val, size_t size)
{
    unsigned char *buf_ptr = (unsigned char *)buf;
    for (size_t i = 0; i < size; i++)
    {
        buf_ptr[i] = (unsigned char)val;
    }

    return buf_ptr;
}