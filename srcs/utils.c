#include "ww.h"

int	_ww_print_errors(enum _ww_e_errors err_code)
{
	const char	*_err_msg_array[] = _WW_ERROR_MSG_ARRAY;
	fprintf(
		stderr,
		_WW_RED_COLOR "Error: %s.\n" _WW_RESET_COLOR,
		_err_msg_array[err_code]
	);
	return 1;
}