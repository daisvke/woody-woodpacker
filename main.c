#include "ww.h"

// This function exits from the program so make sure you do
// what you need to do before calling it.
int	_ww_print_errors(int err_code)
{
	const char	*err_msg_array[] = _WW_ERROR_MSG_ARRAY;
	fprintf(
		stderr,
		_WW_RED_COLOR "Error: %s.\n" _WW_RESET_COLOR,
		err_msg_array[err_code]
	);
	exit(1);
}

int main(int argc, char *argv[]) {
	if (argc != 2) _ww_print_errors(_WW_ERR_BADARGNBR);

    int	_fd = open(argv[1], O_RDONLY);
    if (_fd < 0) _ww_print_errors(_WW_ERR_OPENBIN);
	
	// Determine the file size by moving the cursor till the end
    off_t	_file_size = lseek(_fd, 0, SEEK_END);
	// Put back the cursor at the beginning of the file
    lseek(_fd, 0, SEEK_SET);

    // Allocate memory buffer
    unsigned char*	_buffer = (unsigned char*)malloc(_file_size);
    if (_buffer == NULL) {
        close(_fd);
		_ww_print_errors(_WW_ERR_ALLOCMEM);
    }

    // Read the file contents into memory
    ssize_t	_bytes_read = read(_fd, _buffer, _file_size);
    if (_bytes_read != _file_size) {
        free(_buffer);
        close(_fd);
		_ww_print_errors(_WW_ERR_READFILE);
    }

    close(_fd);
    free(_buffer);

    return 0;
}
