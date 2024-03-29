#ifndef STRING_H
#define STRING_H 1

#include <sys/cdefs.h>
#include <stddef.h>

int memcmp(const void *, const void *, size_t);
void *memcpy(void *__restrict, const void *__restrict, size_t); // Doesn't like overlap
void *memmove(void *, const void *, size_t);                     // Overlap is bearable
void *memset(void *, int, size_t);
size_t strlen(const char *);

#endif