#ifndef STDIO_H
#define STDIO_H 1

#include <sys/cdefs.h>

// Not adding extern C for now -- C++ needs that to interface with this library

int printf(const char *__restrict, ...); //ellipsis - indeterminate number of arguments
/* 
__restrict is used for optimisation information (basically meta qualifier for pointers)
Tells the compiler that this pointer is the only way to access the variable and that the
calling function won't be making any changes to it.
*/
int putchar(int);
int puts(const char *);

#endif