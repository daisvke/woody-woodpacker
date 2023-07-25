#include "ww.h"
#include "stub.h"

unsigned char *_mapped_data;
off_t       _file_size;
uint16_t    _modes;
unsigned char   *_stub = _stubgen;

int main(int argc, char *argv[])
{
    if (argc != 2)
        return _ww_print_errors(_WW_ERR_BADARGNBR);

    if (_ww_parse_argv(argv) == _WW_ERROR)
        return _ww_print_errors(_WW_ERR_BADARG);

    if (_ww_map_file_into_memory(argv[1]) == _WW_ERROR)
        return 1;

    _ww_process_mapped_data();

    return _ww_write_processed_data_to_file();
}
