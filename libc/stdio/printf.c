#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

// Internal function to actually put the string to stdout through putchar
static bool print(const char *data, size_t length)
{
    const unsigned char *str = (const unsigned char *)data;
    for (size_t i = 0; i < length; i++)
    {
        if (putchar(str[i]) == EOF)
        {
            return false;
        }
    }
    return true;
}

int printf(const char *restrict format_str, ...)
{
    va_list params;
    va_start(params, format_str);

    int bytes_written = 0;
    // For now, we'll do string and characters alone
    while (*format_str != '\0')
    {
        size_t max_remaining = INT_MAX - bytes_written;
        // since it consumes a %, we'll ned to enter this condition atleat one character before the %
        if (format_str[0] != '%' || format_str[1] == '%')
        {
            // if the pattern %% is found, we'll want to include the trailing % as part of the string
            if (format_str[0] == '%')
            {
                format_str++; // We start the current substring at the trailing %
            }

            // Take the rest of the string, until a % as a substring and print the whole thing
            size_t subLength = 1; // we start at one, so including the last one is fine
            while (format_str[subLength] && format_str[subLength] != '%')
            {
                subLength++;
            }

            // Check for overflow
            if (max_remaining < subLength)
            {
                // OVERFLOW!
                // set errno
                return -1;
            }

            if (print(format_str, subLength) == false)
            {
                // we have an error; caller can handle this.
                return -1;
            }

            format_str += subLength;
            bytes_written += subLength;
            // now we've consumed the %

            /*if the next character is also a %, we don't want it to be printed by the else condition below,
            without going through the appropriate logic*/
            continue;
        }

        // format[0] is looking at a lone/new % at this point
        format_str++;

        if (*format_str == 'c')
        {
            const char c = (const char)va_arg(params, int);
            if (max_remaining < 1)
            {
                // set errorno
                // OVERFLOW
                return -1;
            }

            if (print(&c, sizeof(c)) == false)
            {
                return -1;
            }

            bytes_written++;
            format_str++;
        }
        else if (*format_str == 's')
        {
            // strings
            const char *str = va_arg(params, const char *);
            size_t len = strlen(str);
            if (max_remaining < len)
            {
                // set errno
                // OVERFLOW
                return -1;
            }

            if (print(str, len) == false)
            {
                return -1;
            }
            bytes_written += len;
            format_str++;
        }
        else
        {
            // Print until the end of the format string
            size_t len = strlen(format_str);
            if (max_remaining < len)
            {
                // set errno
                // OVERFLOW
                return -1;
            }

            if (print(format_str, len) == false)
            {
                return -1;
            }
            format_str += len;
            bytes_written += len;
        }
    }

    va_end(params);
    return bytes_written;
}