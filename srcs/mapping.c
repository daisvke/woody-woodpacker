#include "ww.h"

static int _ww_is_elf64(unsigned char *_mapped_data)
{
    Elf64_Ehdr _elf_header;
    _ww_memcpy(&_elf_header, _mapped_data, sizeof(Elf64_Ehdr));

    // We get to EI_CLASS by moving forward for 4 bytes (= 1st field's size of e_ident)
    if (_elf_header.e_ident[EI_CLASS] != ELFCLASS64)
        return -1;
    return 0;
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
int _ww_map_file_into_memory(const char *filename)
{
    int _fd = open(filename, O_RDONLY);
    if (_fd < 0)
        return _ww_print_errors(_WW_ERR_OPENBIN);

    // Determine the file size by moving the cursor till the end
    _file_size = lseek(_fd, 0, SEEK_END);
    if (_file_size < 0)
        return _ww_print_errors(_WW_ERR_RLSEEK);
    // Put back the cursor at the beginning of the file
    if (lseek(_fd, 0, SEEK_SET < 0))
        return _ww_print_errors(_WW_ERR_RLSEEK);

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
        return _ww_print_errors(_WW_ERR_ALLOCMEM);
    }
    close(_fd); /* No need to keep the fd since the file is mapped */

    if (_ww_is_elf64(_mapped_data) == -1)
        return _ww_print_errors(_WW_ERR_NOT64BITELF);
    return 0;
}

int _ww_write_processed_data_to_file(void)
{
    // 0755: rwx for owner, rx for group and others
    int _outfile_fd = open("woody", O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (_outfile_fd == -1)
    {
        // Unmap the file from memory
        if (munmap(_mapped_data, _file_size) < 0)
            _ww_print_errors(_WW_ERR_MUNMAP);
        return _ww_print_errors(_WW_ERR_OUTFILE);
    }

    size_t _file_size_with_stub = _file_size + ((77/_WW_PAGE_SIZE) +1)*_WW_PAGE_SIZE;//replace by sizeof _stub
    ssize_t _bytes_written = write(_outfile_fd, _mapped_data, _file_size_with_stub);
    if (_bytes_written < 0)
        _ww_print_errors(_WW_ERR_WRITEFILE);
    close(_outfile_fd);

    // Unmap the file from memory
    if (munmap(_mapped_data, _file_size) < 0)
    {
        return _ww_print_errors(_WW_ERR_MUNMAP);
    }
    return 0;
}