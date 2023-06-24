#include "woody-woodpacker.h"

int	print_errors(const char *err_msg)
{
	fprintf(stderr, _WW_RED_COLOR "Error: %s\n" _WW_RESET_COLOR);
	return 1;
}

int main() {
    int	fd = open("input_binary.bin", O_RDONLY);
    if (fd < -1) {
        fprintf(stderr, "Failed to open the binary file.\n");
        return 1;
    }

    // Determine the file size
    off_t	file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    // Allocate memory buffer
    unsigned char*	buffer = (unsigned char*)malloc(file_size);
    if (buffer == NULL) {
        printf("Failed to allocate memory.\n");
        close(fd);
        return 1;
    }

    // Read the file contents into memory
    ssize_t bytes_read = read(fd, buffer, file_size);
    if (bytes_read != file_size) {
        printf("Failed to read the file.\n");
        free(buffer);
        close(fd);
        return 1;
    }

    close(fd);
    free(buffer);

    return 0;
}
