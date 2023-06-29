#include "ww.h"

unsigned char*	_buffer;
off_t	_file_size;

int	_ww_print_errors(int err_code)
{
	const char	*err_msg_array[] = _WW_ERROR_MSG_ARRAY;
	fprintf(
		stderr,
		_WW_RED_COLOR "Error: %s.\n" _WW_RESET_COLOR,
		err_msg_array[err_code]
	);
	return 1;
}

/*
    We grab the e_ident field of the mapped data to get the architecture (32-bit or 64-bit)
    of the file.

    e_ident: This field is an array of bytes that identifies the file as an ELF file.
    It contains specific values at predefined offsets to signify the ELF format version,
    class (32-bit or 64-bit), endianness, OS ABI, and more.

    First fields of e_ident:
    1. EI_MAG0, EI_MAG1, EI_MAG2, EI_MAG3 (4 bytes): These four bytes form the magic number
        and are used to identify an ELF file. The values are usually 0x7F, 'E', 'L', and 'F'.
    2. EI_CLASS (1 byte): This byte specifies the architecture of the file.
*/
bool _ww_is_elf64(unsigned char* _buffer) {
    Elf64_Ehdr  elf_header;
    _ww_memcpy(&elf_header, _buffer, sizeof(Elf64_Ehdr));

    // We get to EI_CLASS by moving forward for 4 bytes (= 1st field's size of e_ident)
    if (elf_header.e_ident[_WW_EI_CLASS] != _WW_ELFCLASS64)
       return false;
    return true;
}

int _ww_map_file_into_memory(const char *filename)
{
    int	_fd = open(filename, O_RDONLY);
    if (_fd < 0) return _ww_print_errors(_WW_ERR_OPENBIN);
	
	// Determine the file size by moving the cursor till the end
    _file_size = lseek(_fd, 0, SEEK_END);
	// Put back the cursor at the beginning of the file
    lseek(_fd, 0, SEEK_SET);

    /* Map the file into memory
		- PROT_READ: read-only
		- MAP_PRIVATE: creates a private copy of the mapped data, so any
		modifications made to the mapped memory will not be visible
		to other processes mapping the same file		
	*/
    _buffer = mmap(NULL, _file_size, PROT_READ, MAP_PRIVATE, _fd, 0);
    if (_buffer == MAP_FAILED) {
        close(_fd);
		return _ww_print_errors(_WW_ERR_ALLOCMEM);
    }
    close(_fd); /* No need to keep the fd since the file is mapped */
    
    if (_ww_is_elf64(_buffer) == false)
        return _ww_print_errors(_WW_ERR_NOT64BITELF);    
    return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 2) return _ww_print_errors(_WW_ERR_BADARGNBR);

    if (_ww_map_file_into_memory(argv[1]) == _WW_ERROR) return 1;

	// Here process the file loaded into memory

    // Unmap the file from memory
    if (munmap(_buffer, _file_size) < 0) {
		return _ww_print_errors(_WW_ERR_MUNMAP);
	}

    return 0;
}
