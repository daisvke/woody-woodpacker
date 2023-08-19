#ifndef ERRORS_H
# define ERRORS_H

# include <errno.h>

# define _WW_ERROR_MSG_ARRAY \
{ \
	"Wrong number of arguments", \
	"Unknown argument(s)", \
	"open: Failed to open the binary file", \
	"malloc: Failed to allocate memory", \
	"mmap: Failed to map the file", \
	"munmap: Failed to unmap the file", \
	"File architecture not suported. x86_64 only", \
	"lseek: Failed to read file", \
	"write: Failed to write to the file", \
	"open: Failed to open/create output file", \
	"No .text section found! Aborting..." \
}

enum	_ww_e_errors
{
	_WW_ERR_BADARGNBR,
	_WW_ERR_BADARG,
	_WW_ERR_OPENBIN,
	_WW_ERR_ALLOCMEM,		
	_WW_ERR_MMAP,
	_WW_ERR_MUNMAP,
	_WW_ERR_NOT64BITELF,
	_WW_ERR_LSEEK,
	_WW_ERR_WRITEFILE,
	_WW_ERR_OUTFILE,
	_WW_ERR_NOTEXTSEC
};

void	_ww_print_error_and_exit(enum _ww_e_errors err_code);

#endif
