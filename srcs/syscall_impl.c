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

// =============================================== //

void *_ww_memset(void *s, int c, size_t n) {
	while (n) {
		*((unsigned char *)s + n - 1) = (unsigned char)c;
		n--;
	}
	return (s);
}

// =============================================== //

size_t	_ww_strcat(char *dest, const char *src) {
	unsigned int i = 0;

	while (dest[i] != '\0')
		i++;
	while (*src != '\0') {
		dest[i] = *src;
		src++;
		i++;
	}
	dest[i] = '\0';
	return _ww_strlen(dest);
}

// =============================================== //

int _ww_strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }

    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// =============================================== //

static char	*protected_malloc(char *str, int size_n) {
	str = malloc(sizeof(char) * size_n + 1);
	if (!str)
		return (0);
	str[size_n] = 0;
	return (str);
}

// =============================================== //

static int	sizeof_n_i(int n) {
	int				size;
	unsigned int	tmp;

	if (n) {
		if (n < 0) {
			size = 1;
			tmp = n * -1;
		}
		else {
			size = 0;
			tmp = n;
		}
		while (tmp) {
			tmp = tmp / 10;
			size++;
		}
	}
	else
		size = 1;
	return (size);
}

static void	inttoascii(char *str, long int tmp, int size_n) {
	if (tmp < 0) {
		tmp = tmp * -1;
		str[0] = '-';
	}
	while (tmp) {
		str[size_n--] = tmp % 10 + 48;
		tmp = tmp / 10;
	}
}

char	*_ww_itoa(int n) {
	int			size_n;
	char		*str;
	long int	tmp;

	str = NULL;
	tmp = n;
	size_n = sizeof_n_i(n) - 1;
	str = protected_malloc(str, sizeof_n_i(n));
	if (n && str)
		inttoascii(str, tmp, size_n);
	else if (!n && str)
		str[size_n] = '0';
	else
		return (0);
	return (str);
}

// =============================================== //

static int	sizeof_n_u(unsigned int n) {
	int				size;
	unsigned int	tmp;

	if (n) {
		size = 0;
		tmp = n;
		while (tmp) {
			tmp = tmp / 10;
			size++;
		}
	}
	else
		size = 1;
	return (size);
}

static void	unstoascii(char *str, unsigned long int tmp, int size_n) {
	while (tmp) {
		str[size_n--] = tmp % 10 + 48;
		tmp = tmp / 10;
	}
}

char	*_ww_utoa(unsigned int n) {
	int					size_n;
	unsigned long int	tmp;
	char				*str;

	str = NULL;
	tmp = n;
	size_n = sizeof_n_u(n) - 1;
	str = protected_malloc(str, sizeof_n_u(n));
	if (n && str)
		unstoascii(str, tmp, size_n);
	else if (!n && str)
		str[size_n] = '0';
	else
		return (0);
	return (str);
}

// =============================================== //

static int	len_str(unsigned long nb) {
	int	len;

	if (nb == 0)
		return (1);
	len = 0;
	while (nb) {
		nb = nb / 16;
		len++;
	}
	return (len);
}

char	*_ww_utohex(unsigned long nb) {
	char	*str;
	char	*hexbase_low;
	int		len;

	hexbase_low = "0123456789abcdef";
	len = len_str(nb);
	str = (char *)malloc(sizeof(char) * len + 1);
	if (str == NULL)
		return (NULL);
	if (nb == 0)
		str[0] = '0';
	str[len] = 0;
	while (nb) {
		str[len - 1] = *(hexbase_low + nb % 16);
		nb = nb / 16;
		len--;
	}
	return (str);
}

// =============================================== //

char* unsignedlong_to_hex_string(unsigned long number) {
	int length = 0;
	unsigned long temp = number;

	// Calculate the length of the resulting hexadecimal string
	do {
		length++;
		temp >>= 4;  // Shift right by 4 bits (equivalent to dividing by 16)
	} while (temp != 0);

	char* str = malloc((length + 1) * sizeof(char));  // Allocate memory for the string

	// Convert the number to a hexadecimal string
	str[length] = '\0';  // Add null terminator

	// Build the hexadecimal string from right to left
	while (length-- > 0) {
		int remainder = number & 0xF;  // Extract the lowest 4 bits
		str[length] = (remainder < 10) ? ('0' + remainder) : ('a' + remainder - 10);
		number >>= 4;  // Shift right by 4 bits
	}

	return str;
}

// =============================================== //

char *convert_ulong_to_hex(unsigned long number, bool padding) {
	char* str = unsignedlong_to_hex_string(number);

	if (padding) {
		int paddingLength = 16 - _ww_strlen(str);
		if (paddingLength > 0) {
			char* paddedStr = malloc((16 + 1) * sizeof(char));

			_ww_memset(paddedStr, '0', paddingLength);
			paddedStr[paddingLength] = '\0';

			_ww_strcat(paddedStr, str);
			free(str);

			return paddedStr;
		}
	}

	return str;
}

// =============================================== //

void _ww_dprintf(int fd, char *fmt, ...) {
	va_list var;
	va_start(var, fmt);
	while (*fmt) {
		char c = *fmt++;
		if (c == '%') {
			bool padding = false;
			if (*fmt == '0') {
				padding = true;
				fmt++;
			}
			switch (*fmt++) {
				char *s;
				case 'c':
					char c = va_arg(var, int);
					write(fd, &c, 1);
					break;
				case 's':
					s = va_arg(var, char *);
					write(fd, s, _ww_strlen(s));
					break;
				case 'p':
					void *p = va_arg(var, char *);
					s = _ww_utohex((unsigned long)p);
					write(fd, s, _ww_strlen(s));
					free(s);
					break;
				case 'd':
					int i = va_arg(var, int);
					s = _ww_itoa(i);
					write(fd, s, _ww_strlen(s));
					free(s);
					break;
				case 'u':
					unsigned int u = va_arg(var, unsigned int);
					s = _ww_utoa(u);
					write(fd, s, _ww_strlen(s));
					free(s);
					break;
				case 'x':
					unsigned int x = va_arg(var, unsigned int);
					s = _ww_utohex((unsigned long)x);
					write(fd, s, _ww_strlen(s));
					free(s);
					break;
				case 'l':
					if (*fmt++ == 'x') {
						unsigned int x = va_arg(var, unsigned int);
						s = convert_ulong_to_hex((unsigned long)x, padding);
						write(fd, s, _ww_strlen(s));
						free(s);
						break;
					}
			}
		}
		else
			write(fd, &c, 1);
	}
	va_end(var);
	// An improvment of the function is to return the length of the printed string
}