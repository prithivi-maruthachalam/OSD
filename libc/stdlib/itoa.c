#include <stdlib.h>
#include <stdint.h>

char *itoa(unsigned long int num, char *buf, unsigned int radix)
{
    int i = 0;
    int j = 0;

    if (num == 0)
    {
        buf[i++] = '0';
        buf[i] = '\0';
        return buf;
    }

    // Don't really need signed support for other radices
    if (radix == 10 && (signed long int)num < 0)
    {
        num = 0 - num;
        if (radix == 10)
        {
            buf[i++] = '-';
            j++;
        }
    }

    while (num > 0)
    {
        uint32_t digit = (radix != 10) ? (uint32_t)num % radix : num % radix;
        buf[i] = (digit > 9) ? 'A' + (digit - 10) : (digit) + '0';
        num = num / radix;
        i++;
    }
    buf[i] = '\0';

    while (i > j)
    {
        char temp = buf[j];
        buf[j++] = buf[--i];
        buf[i] = temp;
    }
    return buf;
}