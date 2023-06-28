#ifndef ERRORS_H
# define ERRORS_H

# define _WW_ERROR_MSG_ARRAY \
{ \
	"Failed to open the binary file", \
	"Failed to allocate memory", \
	"Failed to read the file", \
	"Wrong number of arguments" \
}

# define _WW_ERR_OPENBIN		0
# define _WW_ERR_ALLOCMEM		1
# define _WW_ERR_READFILE		2
# define _WW_ERR_BADARGNBR		3

#endif
