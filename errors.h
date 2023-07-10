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
	"File architecture not suported. x86_64 only", \
	"lseek: Failed to read file", \
	"Failed to open the file", \
	"Failed to write to the file" \
}

enum	_ww_e_errors
{
	_WW_ERR_BADARGNBR,
	_WW_ERR_BADARG,
	_WW_ERR_OPENBIN,
	_WW_ERR_ALLOCMEM,		
	_WW_ERR_READFILE,		
	_WW_ERR_MUNMAP,
	_WW_ERR_NOT64BITELF,
	_WW_ERR_RLSEEK,
	_WW_ERR_OUTFILE,
	_WW_ERR_WRITEFILE
};

void    *_ww_memcpy(void *dest, const void *src, size_t n);

#endif
