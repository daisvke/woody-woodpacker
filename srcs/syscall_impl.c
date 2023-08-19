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

void    *_ww_memset(void *src, int c, size_t n)
{
        size_t  i;
        char    *str;

        str = src;
        i = 0;
        while (i < n) str[i++] = (unsigned char)c;
        return (src);
}