#include "ww.h"

unsigned char *_mapped_data;
Elf64_Off _file_size;
uint16_t _modes;

int main(int argc, char *argv[])
{
    if (argc != 2)
        _ww_print_error_and_exit(_WW_ERR_BADARGNBR);

    if (_ww_parse_argv(argv) == _WW_ERROR)
        _ww_print_error_and_exit(_WW_ERR_BADARG);

    _ww_map_file_into_memory(argv[1]);
    _ww_process_mapped_data();
    _ww_write_processed_data_to_file();
}
