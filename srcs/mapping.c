#include "ww.h"

// Check if a file is 64 bits ELF file
static int  ww_is_elf64(unsigned char *g_mapped_data)
{
    Elf64_Ehdr  elf_header;
    ww_memcpy(&elf_header, g_mapped_data, sizeof(Elf64_Ehdr));

    // We get to EI_CLASS by moving forward for 4 bytes
    // (= 1st field's size of e_ident)
    if (elf_header.e_ident[EI_CLASS] != ELFCLASS64) return -1;
    return 0;
}

void    ww_map_file_into_memory(const char *filename)
{
    // Open the target binary file
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        ww_print_error_and_exit(WW_ERR_OPENBIN);

    // Determine the file size by moving the cursor till the end
    int res = lseek(fd, 0, SEEK_END);
    if (res < 0)
        ww_print_error_and_exit(WW_ERR_LSEEK);
    else g_file_size = res; // Assign the filesize to the global variable
    // Put back the cursor at the beginning of the file
    if (lseek(fd, 0, SEEK_SET < 0))
        ww_print_error_and_exit(WW_ERR_LSEEK);

    /* Map the file into memory
        - PROT_READ: read-only access
        - PROT_WRITE: write-only access
            We use both READ and WRITE since we are going to encrypt the
            mapped region directly.
        - MAP_PRIVATE: creates a private copy of the mapped data, so any
        modifications made to the mapped memory will not be visible
        to other processes mapping the same file
    */
    g_mapped_data =
        mmap(NULL, g_file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (g_mapped_data == MAP_FAILED)
    {
        close(fd);
        ww_print_error_and_exit(WW_ERR_MMAP);
    }
    close(fd); /* No need to keep the fd since the file is mapped */

    // Quit if the target is not an ELF 64 bit binary
    if (ww_is_elf64(g_mapped_data) == -1)
        ww_print_error_and_exit(WW_ERR_NOT64BITELF);
}

// Write the processed file data back to the file
void    ww_write_processed_data_to_file(void)
{
    // 0755: rwx for owner, rx for group and others
    int outfilefd = open(WW_PACKED_FILENAME, O_CREAT | O_RDWR | O_TRUNC, 0755);
    if (outfilefd == -1)
    {
        // Unmap the file from memory only if the injection was made into the
		//  segment's padding (otherwise it is unmapped in a different section
        if (g_modes & WW_INJECTREG_PADDING)
            if (munmap(g_mapped_data, g_file_size) < 0)
                ww_print_error_and_exit(WW_ERR_MUNMAP);
        ww_print_error_and_exit(WW_ERR_OUTFILE);
    }

    // Write the processed data to the outfile
    ssize_t bytes_written = write(outfilefd, g_mapped_data, g_file_size);
    if (bytes_written < 0)
        ww_print_error_and_exit(WW_ERR_WRITEFILE);
    if (!(g_modes & WW_INJECTREG_PADDING))
        free(g_mapped_data);
    else // Unmap the file from memory
        if (munmap(g_mapped_data, g_file_size) < 0)
            ww_print_error_and_exit(WW_ERR_MUNMAP);
    close(outfilefd);
}
