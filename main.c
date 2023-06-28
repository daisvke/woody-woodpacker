#include "woody-woodpacker.h"

int	_ww_print_errors(const char *err_msg)
{
	fprintf(stderr, _WW_RED_COLOR "Error: %s\n" _WW_RESET_COLOR, err_msg);
	return 1;
}

int main() {
    int	_fd = open("input_binary.bin", O_RDONLY);
    if (_fd < -1) {
        fprintf(stderr, "Failed to open the binary file.\n");
        return 1;
    }

    // Determine the file size by moving the cursor till the end
    off_t	_file_size = lseek(_fd, 0, SEEK_END);
	// Put back the cursor at the beginning of the file
    lseek(_fd, 0, SEEK_SET);

    // Allocate memory buffer
    unsigned char*	_buffer = (unsigned char*)malloc(_file_size);
    if (_buffer == NULL) {
        printf("Failed to allocate memory.\n");
        close(_fd);
        return 1;
    }

    // Read the file contents into memory
    ssize_t _bytes_read = read(_fd, _buffer, _file_size);
    if (_bytes_read != _file_size) {
        printf("Failed to read the file.\n");
        free(_buffer);
        close(_fd);
        return 1;
    }

    close(_fd);

    free(_buffer);

    return 0;
}
