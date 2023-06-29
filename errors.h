#ifndef ERRORS_H
# define ERRORS_H

# define _WW_ERROR_MSG_ARRAY \
{ \
	"Failed to open the binary file", \
	"Failed to allocate memory", \
	"Failed to read the file", \
	"Wrong number of arguments" \
}

enum	_ww_e_errors
{
	_WW_ERR_OPENBIN,
	_WW_ERR_ALLOCMEM,		
	_WW_ERR_READFILE,		
	_WW_ERR_BADARGNBR,		
};

#endif
