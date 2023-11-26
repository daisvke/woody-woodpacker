#include "ww.h"

unsigned char   *g_mapped_data;
Elf64_Off       g_file_size;
uint16_t        g_modes;

int main(int argc, char *argv[])
{
    if (argc != 2)
        ww_print_error_and_exit(WW_ERR_BADARGNBR);

    if (ww_parse_argv(argv) == WW_ERROR)
        ww_print_error_and_exit(WW_ERR_BADARG);

    ww_map_file_into_memory(argv[1]);
    ww_process_mapped_data();
    ww_write_processed_data_to_file();
}
