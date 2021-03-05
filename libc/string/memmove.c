#include <string.h>

void *memmove(void *dest, const void *src, size_t size)
{
    const unsigned char *src_ptr = (const unsigned char *)src;
    unsigned char *dest_ptr = (unsigned char *)dest;

    // We're making sure that it's always src that gets overwritten
    if (dest < src)
    {
        for (size_t i = 0; i < size; i++)
        {
            dest_ptr[i] = src_ptr[i];
        }
    }
    else
    {
        for (size_t i = size; i > 0; i--)
        {
            dest_ptr[i - 1] = src_ptr[i - 1];
        }
    }

    return dest_ptr;
}