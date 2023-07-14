#include "ww.h"

int _ww_parse_argv(char *argv[])
{
	// ----------------------For testing----------------------------
	// Region from the source file to encrypt
	_modes |= _WW_CYPTREG_PHDR,			// Select region using segments
	// _modes |= _WW_CYPTREG_PHTEXTX;		// .text segments with x rights
	// _modes |= _WW_CYPTREG_PHTEXT,		// .text segments
	_modes |= _WW_CYPTREG_PHALL,			// All segments
	// _modes |= _WW_CYPTREG_SHDR, // Select region using sections
	// _modes |= _WW_CYPTREG_SHTEXT,	 	// text section
	// _modes |= _WW_CYPTREG_SHDATA,	 	// data section
	// _modes |= _WW_CYPTREG_SHALL			// text + data sections

	// Unpacker and stub insertion region in the target file
	_modes |= _WW_INJTREG_PAD; // Insert inside segments paddings
	// _modes |= _WW_INJTREG_SFT,			// Insert at 0x0 and shift the rest
	// _modes |= _WW_INJTREG_END			// Insert at the end of the file
	// --------------------------------------------------------------
	return 0;
}