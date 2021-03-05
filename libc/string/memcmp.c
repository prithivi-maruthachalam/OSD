#include <string.h>

// Compares first n bytes of 2 pointers
int memcmp(const void *memA, const void *memB, size_t size)
{
    // Cast the pointer to be used a 1 byte elements
    const unsigned char *a = (const unsigned char *)memA;
    const unsigned char *b = (const unsigned char *)memB;

    for (size_t i = 0; i < size; i++)
    {
        if (a[i] < b[i])
            return -1;
        else if (a[i] > b[i])
            return 1;
    }

    // First n bytes are equal
    return 0;
}