#include "ww.h"

unsigned char   *g_mapped_data;
Elf64_Off       g_file_size;
uint16_t        g_modes;

int main(int argc, char *argv[])
{
	(void)argc;

    ww_parse_argv(argv);

	char *filename = ww_get_filename(argv);
	if (!filename)
		ww_print_error_and_exit(WW_ERR_BADARGNBR);

    ww_map_file_into_memory(filename);
    ww_process_mapped_data();
    ww_write_processed_data_to_file();
}
