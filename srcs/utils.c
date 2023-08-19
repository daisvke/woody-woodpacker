#include "ww.h"

void	_ww_print_error_and_exit(enum _ww_e_errors err_code)
{
	const char	*_err_msg_array[] = _WW_ERROR_MSG_ARRAY;
	fprintf(
		stderr,
		_WW_RED_COLOR "Error: %s.\n" _WW_RESET_COLOR,
		_err_msg_array[err_code]
	);
	exit(1);
}