#include "ww.h"

int ww_parse_argv(char *argv[])
{
	(void)argv;
	g_modes |= WW_VERBOSE;
	return 0;
}