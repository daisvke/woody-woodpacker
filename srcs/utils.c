#include "ww.h"

static bool ww_in_error_and_exit = false;

void ww_print_error_and_exit(enum ww_e_errors err_code)
{
    const char *err_msg_array[] = WW_ERROR_MSG_ARRAY;

    if (ww_in_error_and_exit) {
        fprintf(stderr, "Error: Recursive call to ww_print_error_and_exit.\n");
        exit(1);
    }

    ww_in_error_and_exit = true;

    fprintf(
        stderr,
        WW_RED_COLOR "Error: %s.\n" WW_RESET_COLOR,
        err_msg_array[err_code]
    );

    if (g_mapped_data)
        if (munmap(g_mapped_data, g_file_size) < 0)
            fprintf(stderr, "Error: Unable to unmap memory.\n");

    ww_in_error_and_exit = false;

    exit(1);
}
