#include "ww.h"

unsigned char   *g_mapped_data;
Elf64_Off       g_file_size;
uint16_t        g_modes;

int main(int argc, char *argv[])
{
	(void)argc;
	// Parse the arguments given through the commannd line
    ww_parse_argv(argv);

	// Get the target binary filename from the given arguments
	char *filename = ww_get_filename(argv);
	if (!filename)
		ww_print_error_and_exit(WW_ERR_BADARGNBR);

	/* Create a mapping between the target file and the memory region occupied
	 * by the program. This is to facilitate the memory handling to manipulate
	 * the file data.
	 */
    ww_map_file_into_memory(filename);
	// Process the encryption, the injection, etc
    ww_process_mapped_data();
	// Write the processed data back into the file
    ww_write_processed_data_to_file();
}
