#include "ww.h"

static bool _ww_in_error_and_exit = false;

void _ww_print_error_and_exit(enum _ww_e_errors err_code)
{
    const char *_err_msg_array[] = _WW_ERROR_MSG_ARRAY;

    if (_ww_in_error_and_exit) {
        fprintf(stderr, "Error: Recursive call to _ww_print_error_and_exit.\n");
        exit(1);
    }

    _ww_in_error_and_exit = true;

    fprintf(
        stderr,
        _WW_RED_COLOR "Error: %s.\n" _WW_RESET_COLOR,
        _err_msg_array[err_code]
    );

    if (_mapped_data)
        if (munmap(_mapped_data, _file_size) < 0)
            fprintf(stderr, "Error: Unable to unmap memory.\n");

    _ww_in_error_and_exit = false;

    exit(1);
}
