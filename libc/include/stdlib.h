#ifndef STDLIB_H
#define STDLIB_H 1

#include <sys/cdefs.h>

// Kernel panic - abnormally terminate process like SIGABRT
__attribute__((__noreturn__)) void abort();

char *itoa(int, char *, unsigned int);

#endif