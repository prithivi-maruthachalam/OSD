#include <string.h>

// the restricted pointer is the only one that is allowed to acces that object in memory
// here, it discourages overlap. Same memory location can't be accessed by both dest and src
void *memcpy(void *restrict dest, const void *restrict src, size_t size)
{
    const unsigned char *src_ptr = (const unsigned *)src;
    unsigned char *dest_ptr = (unsigned *)dest;

    for (size_t i = 0; i < size; i++)
    {
        dest_ptr[i] = src_ptr[i];
    }

    return dest_ptr;
}