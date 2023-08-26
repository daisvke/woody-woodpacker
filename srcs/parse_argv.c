#include "ww.h"

int _ww_parse_argv(char *argv[])
{
	(void)argv;
	_modes |= _WW_VERBOSE;
	return 0;
}