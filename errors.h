#ifndef WW_ERRORS_H
# define WW_ERRORS_H

# include <errno.h>
# include <stdbool.h>

# define WW_ERROR_MSG_ARRAY \
{ \
	"Wrong number of arguments", \
	"Unknown argument(s)", \
	"open: Failed to open the binary file", \
	"malloc: Failed to allocate memory", \
	"mmap: Failed to map the file", \
	"munmap: Failed to unmap the file", \
	"File architecture not suported. 64-bit ELF files only", \
	"lseek: Failed to read file", \
	"write: Failed to write to the file", \
	"open: Failed to open/create output file", \
	"No .text section found! Aborting...", \
	"Corrupted program header", \
	"Corrupted ELF header", \
	"Unrecognized option", \
}

enum	ww_e_errors
{
	WW_ERR_BADARGNBR,
	WW_ERR_BADARG,
	WW_ERR_OPENBIN,
	WW_ERR_ALLOCMEM,		
	WW_ERR_MMAP,
	WW_ERR_MUNMAP,
	WW_ERR_NOT64BITELF,
	WW_ERR_LSEEK,
	WW_ERR_WRITEFILE,
	WW_ERR_OUTFILE,
	WW_ERR_NOTEXTSEC,
	WW_ERR_CORRUPTPHDR,
	WW_ERR_CORRUPTEHDR,
	WW_ERR_UNRECOGNIZEDOPT,
};

void	ww_print_error_and_exit(enum ww_e_errors err_code);

#endif
