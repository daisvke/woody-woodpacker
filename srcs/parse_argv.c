#include "ww.h"

int _ww_parse_argv(char *argv[])
{
	(void)argv;
	// ----------------------For testing----------------------------
	// Region from the source file to encrypt
	_modes |= _WW_CRYPTREG_PHDR;			// Select region using segments
	// _modes |= _WW_CRYPTREG_PHTEXTX;		// .text segments with x rights
	// _modes |= _WW_CRYPTREG_PHTEXT,		// .text segments
	_modes |= _WW_CRYPTREG_PHALL;			// All segments
	// _modes |= _WW_CRYPTREG_SHDR, // Select region using sections
	// _modes |= _WW_CRYPTREG_SHTEXT,	 	// text section
	// _modes |= _WW_CRYPTREG_SHDATA,	 	// data section
	// _modes |= _WW_CRYPTREG_SHALL			// text + data sections
	// --------------------------------------------------------------
	return 0;
}