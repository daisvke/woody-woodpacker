#include "ww.h"
#include <stdio.h> // NOTE: to remove


// TODO: Write nice documentation to what happens in this file
// TODO: Make it possible to encrypt multiple files
// TODO: For each file, print the result (success or failure)
// TODO: Print selected options, ex: `Padding mode choose, segment encryption.`
// TODO: No options choosed, we should display a default message like :
// `No options choosed, trying from more optimized method to less, encrypting all the segments`
// TODO: For each file: while they are being processed, print debug information like:
// `Trying padding mode`, `Failed padding mode, trying shifting mode`, etc...
// TODO: If no options, no file: default to a.out

void _ww_help(char *progname, int fd) {
	dprintf(fd, RESET"Usage: %s: [option(s)] [file(s)]\n", progname);
	dprintf(fd, "Pack [files(s)] by encrypting sections or segments (a.out by default).\n");
	dprintf(fd, "The options are: \n");
	dprintf(fd, "-rlevel			Select the region to encrypt\n");
	// TODO: Explain for each level what will be encrypted here
	// TODO: Add an underline to `level`
	dprintf(fd, "-p, --padding		Select the padding injection method\n");
	dprintf(fd, "-s, --shifting		Select the shifting injection method\n");
	// TODO: Maybe be more clear about the code injection methods (?)
	dprintf(fd, "-h, --help		Display this information\n");
}

int _ww_get_opt(char *argv[])
{
	printf(MAG"[   DEV   ]\n"RESET);
	_ww_help(argv[0], STDOUT_FILENO);
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
	printf(MAG"[ END DEV ]\n"RESET);
	return 0;
}

void debug_print_bits(uint16_t num) {
    int size = sizeof(num) * 8; // Assuming an integer has 32 bits (on most systems)
    for (int i = size - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}
