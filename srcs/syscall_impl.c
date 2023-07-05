#include "ww.h"

void    *_ww_memcpy(void *dest, const void *src, size_t n)
{
        char            *d;
        const char      *s;

        d = dest;
        s = src;
        if (!dest && !src)
                return (NULL);
        while (n--) *d++ = *s++;
        return (dest);
}

size_t  _ww_strlen(const char *s)
{
        size_t  i;

        i = 0;
        while (s[i]) ++i;
        return (i);
}