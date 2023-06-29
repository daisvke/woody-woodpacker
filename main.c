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
#define EI_CLASS    4
#define ELFCLASS64  2

int is_elf_64(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("Failed to open file");
        return 0;
    }

    // Read the first 5 bytes of the file to access the ELF identification
    uint8_t ident[EI_CLASS + 1];
    if (read(file, ident, EI_CLASS + 1) != EI_CLASS + 1) {
        close(file);
        return 0;
    }

    close(file);

    // Check if the file has the correct ELF identification for a 64-bit file
    if (ident[0] == 0x7F && ident[1] == 'E' && ident[2] == 'L' && ident[3] == 'F' && ident[EI_CLASS] == ELFCLASS64) {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 2) _ww_print_errors(_WW_ERR_BADARGNBR);

    int	_fd = open(argv[1], O_RDONLY);
    if (_fd < 0) _ww_print_errors(_WW_ERR_OPENBIN);
	
	// Determine the file size by moving the cursor till the end
    off_t	_file_size = lseek(_fd, 0, SEEK_END);
	// Put back the cursor at the beginning of the file
    lseek(_fd, 0, SEEK_SET);

    /* Map the file into memory
		- PROT_READ: read-only
		- MAP_PRIVATE: creates a private copy of the mapped data, so any
		modifications made to the mapped memory will not be visible
		to other processes mapping the same file		
	*/
    unsigned char*	_buffer = 
		mmap(NULL, _file_size, PROT_READ, MAP_PRIVATE, _fd, 0);
    if (_buffer == MAP_FAILED) {
        close(_fd);
		_ww_print_errors(_WW_ERR_ALLOCMEM);
    }

	// Here process the file loaded into memory

    if (munmap(_buffer, _file_size) < 0) {
    	close(_fd);
		_ww_print_errors(_WW_ERR_MUNMAP);
	}
    close(_fd);

    return 0;
}
