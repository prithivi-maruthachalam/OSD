#include <stdlib.h>

char *itoa(int num, char *buf, unsigned int radix)
{
    int i = 0;
    int j = 0;

    if (num == 0)
    {
        buf[i++] = '0';
        buf[i] = '\0';
        return buf;
    }

    if (num < 0)
    {
        num = 0 - num;
        buf[i++] = '-';
        j++;
    }

    while (num > 0)
    {
        buf[i] = (char)((num % radix) + '0');
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