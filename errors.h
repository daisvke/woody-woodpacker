#ifndef ERRORS_H
# define ERRORS_H

# include <errno.h>

# define _WW_ERROR_MSG_ARRAY \
{ \
	"Failed to open the binary file", \
	"mmap: Failed to map the file", \
	"Failed to read the file", \
	"Wrong number of arguments", \
	"munmap: Failed to unmap the file", \
	"Given file is not 64 ELF", \
	"lseek: Failed to read file" \
}

enum	_ww_e_errors
{
	_WW_ERR_OPENBIN,
	_WW_ERR_ALLOCMEM,		
	_WW_ERR_READFILE,		
	_WW_ERR_BADARGNBR,
	_WW_ERR_MUNMAP,
	_WW_ERR_NOT64BITELF,
	_WW_ERR_RLSEEK
};

void    *_ww_memcpy(void *dest, const void *src, size_t n);

#endif
