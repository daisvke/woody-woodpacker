#include "ww.h"

static int _ww_is_elf64(unsigned char *_mapped_data)
{
    Elf64_Ehdr _elf_header;
    _ww_memcpy(&_elf_header, _mapped_data, sizeof(Elf64_Ehdr));

    // We get to EI_CLASS by moving forward for 4 bytes
    // (= 1st field's size of e_ident)
    if (_elf_header.e_ident[EI_CLASS] != ELFCLASS64)
        return -1;
    return 0;
}

void _ww_map_file_into_memory(const char *filename)
{
    int _fd = open(filename, O_RDONLY);
    if (_fd < 0)
        _ww_print_error_and_exit(_WW_ERR_OPENBIN);

    // Determine the file size by moving the cursor till the end
    int res = lseek(_fd, 0, SEEK_END);
    if (res < 0)
        _ww_print_error_and_exit(_WW_ERR_LSEEK);
    else _file_size = res;
    // Put back the cursor at the beginning of the file
    if (lseek(_fd, 0, SEEK_SET < 0))
        _ww_print_error_and_exit(_WW_ERR_LSEEK);

    /* Map the file into memory
        - PROT_READ: read-only access
        - PROT_WRITE: write-only access
            We use both READ and WRITE since we are going to encrypt the
            mapped region directly.
        - MAP_PRIVATE: creates a private copy of the mapped data, so any
        modifications made to the mapped memory will not be visible
        to other processes mapping the same file
    */
    _mapped_data =
        mmap(NULL, _file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, _fd, 0);
    if (_mapped_data == MAP_FAILED)
    {
        close(_fd);
        _ww_print_error_and_exit(_WW_ERR_MMAP);
    }
    close(_fd); /* No need to keep the fd since the file is mapped */

    if (_ww_is_elf64(_mapped_data) == -1)
        _ww_print_error_and_exit(_WW_ERR_NOT64BITELF);
}

void    _ww_write_processed_data_to_file(void)
{
    // 0755: rwx for owner, rx for group and others
    int _outfile_fd = open(_WW_PACKED_FILENAME, O_CREAT | O_RDWR | O_TRUNC, 0755);
    if (_outfile_fd == -1)
    {
        // Unmap the file from memory
        if (_modes & _WW_INJECTREG_PADDING)
            if (munmap(_mapped_data, _file_size) < 0)
                _ww_print_error_and_exit(_WW_ERR_MUNMAP);
        _ww_print_error_and_exit(_WW_ERR_OUTFILE);
    }

    // Write the processed data to the outfile
    ssize_t _bytes_written = write(_outfile_fd, _mapped_data, _file_size);
    if (_bytes_written < 0)
        _ww_print_error_and_exit(_WW_ERR_WRITEFILE);
    if (!(_modes & _WW_INJECTREG_PADDING))
        free(_mapped_data);
    else // Unmap the file from memory
        if (munmap(_mapped_data, _file_size) < 0)
            _ww_print_error_and_exit(_WW_ERR_MUNMAP);
    close(_outfile_fd);
}