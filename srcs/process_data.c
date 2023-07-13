#include "ww.h"

int	write_woody_to_file(void* towrite, size_t woodysz)
{
    // 0755: rwx for owner, rx for group and others
    int _outfile_fd = open("woody", O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (_outfile_fd == -1) {
        // Unmap the file from memory
        return _ww_print_errors(_WW_ERR_OUTFILE);
    }

    ssize_t _bytes_written = write(_outfile_fd, towrite, woodysz);
    if (_bytes_written < 0) _ww_print_errors(_WW_ERR_WRITEFILE);
    close(_outfile_fd);

    return 0;
}

void	_ww_process_mapped_data()
{
	void *woody;
	size_t woodysz;

	/* Section injection method selected */
	woody = inject_section(&woodysz);

	if (woody == NULL)
		return;

	/* Create the woody binary in the disk */
	write_woody_to_file(woody, woodysz);
	free(woody);
}
