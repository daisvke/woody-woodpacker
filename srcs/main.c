#include "ww.h"

unsigned char   *g_mapped_data;
Elf64_Off       g_file_size;
uint16_t        g_modes;
char            *g_progname;
int             g_elf_class;

int main(int argc, char *argv[])
{
	(void)argc;
	g_progname = argv[0];

    ww_parse_argv(argv);

	if (g_modes & WW_HELP)
		ww_help(stdout);

	char *filename = ww_get_filename(argv);
	if (!filename)
		ww_print_error_and_exit(WW_ERR_BADARGNBR);

    ww_map_file_into_memory(filename);
    ww_process_mapped_data();
    ww_write_processed_data_to_file();
}
