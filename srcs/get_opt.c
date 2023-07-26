#include "ww.h"

// NOTE: // _WW_INJTREG_END // Insert at the end of the file => is not handled

static bool _ww_is_long_opt(char *arg) {
	return (arg[0] != '\0' && arg[1] != '\0' && arg[0] == '-' && arg[1] == '-');
}

static bool _ww_is_short_opt(char *arg) {
	return (arg[0] != '\0' && arg[0] == '-');
}

bool _ww_is_option_set(uint16_t *options, uint16_t option_flag) {
	return (*options & option_flag) != 0;
}

static void set_options(uint16_t *options, uint16_t opt_flag) {
	*options |= opt_flag;
}

static _ww_option* get_options(int *count) {
	static _ww_option options[] = {
		{'r', ""},
		{'p', "padding"},
		{'s', "shifting"},
		{'h', "help"},
	};

	if (count)
		*count = sizeof(options) / sizeof(options[0]);
	return options;
}

static void process_short_opt(char *arg, uint16_t *_modes, char *progname) {
	int count;
	_ww_option *options = get_options(&count);

	int arg_len = _ww_strlen(arg);
	for (int i = 1; i < arg_len; i++) { // Iterate over program params
		for (int j = 0; j < count; j++) { // Iterate over available options
			if (arg[i] == options[j].short_opt) {
				 uint16_t opt_flag = 0;
				if (options[j].short_opt == 'r') {
					i++;
					if (arg[i] != '0' && arg[i] != '1' &&
						arg[i] != '2' && arg[i] != '3' &&
						arg[i] != '4' && arg[i] != '5' &&
						arg[i] != '6' && arg[i] != '7') {
						_ww_dprintf(STDERR_FILENO, "%s: unrecognized region level '%c'\n", progname, arg[i]);
						set_options(_modes, _WW_MODE_ERROR);
					}
					else {
						if (arg[i] == '0')
							// Select region using segments
							opt_flag = _WW_CYPTREG_PHDR;
						else if (arg[i] == '1')
							// .text segments with x rights
							opt_flag = _WW_CYPTREG_PHTEXTX;
						else if (arg[i] == '2')
							// .text segments
							opt_flag = _WW_CYPTREG_PHTEXT;
						else if (arg[i] == '3')
							// All segments
							opt_flag = _WW_CYPTREG_PHALL;
						else if (arg[i] == '4')
							// Select region using sections
							opt_flag = _WW_CYPTREG_SHDR;
						else if (arg[i] == '5')
							// text section
							opt_flag = _WW_CYPTREG_SHTEXT;
						else if (arg[i] == '6')
							// data section
							opt_flag = _WW_CYPTREG_SHDATA;
						else if (arg[i] == '7')
							// text + data sections
							opt_flag = _WW_CYPTREG_SHALL;
					}
				}
				else if (options[j].short_opt == 'p')
					// Insert inside segments paddings
					opt_flag = _WW_INJTREG_PAD;
				else if (options[j].short_opt == 's')
					// Insert at 0x0 and shift the rest
					opt_flag = _WW_INJTREG_SFT;
				else if (options[j].short_opt == 'h')
					// Print the help message
					opt_flag = _WW_HELP;
				set_options(_modes, opt_flag);
				break;
			}
			else if (j + 1 == count) {
				set_options(_modes, _WW_MODE_ERROR); // option_error
				_ww_dprintf(STDERR_FILENO, "%s: unrecognized option '%c'\n", progname, arg[i]);
				return;
			}
		}
	}
}

static void process_long_opt(char *arg, uint16_t *_modes, char *progname) {
	int count;
	_ww_option *options = get_options(&count);

	for (long i = 0; i < count; i++) {
		if (_ww_strcmp(arg + 2, options[i].long_opt) == 0) {
			uint16_t opt_flag = 0;
			// if (options[i].short_opt == 'r') { }
			if (options[i].short_opt == 'p')
				opt_flag = _WW_INJTREG_PAD;
			else if (options[i].short_opt == 's')
				opt_flag = _WW_INJTREG_SFT;
			else if (options[i].short_opt == 'h')
				opt_flag = _WW_HELP;
			set_options(_modes, opt_flag);
			return;
		}
	}
	set_options(_modes, _WW_MODE_ERROR);
	_ww_dprintf(STDERR_FILENO, "%s: unrecognized option '%s'\n", progname, arg);
}

void _ww_help(char *progname, int fd) {
	_ww_dprintf(fd, "Usage: %s: [option(s)] [file(s)]\n", progname);
	_ww_dprintf(fd, "Pack [files(s)] by encrypting sections or segments (a.out by default).\n");
	_ww_dprintf(fd, " The options are: \n");
	_ww_dprintf(fd, "  -r\e[4mlevel\e[0m,	--region=\e[4mlevel\e[0m		Select the region to encrypt\n");
	// TODO: Explain for each level what will be encrypted here
	// TODO: Add an underline to `level`
	_ww_dprintf(fd, "  -p,		--padding		Select the padding injection method\n");
	_ww_dprintf(fd, "  -s,		--shifting		Select the shifting injection method\n");
	// TODO: Maybe be more clear about the code injection methods (?)
	_ww_dprintf(fd, "  -h,		--help			Display this information\n");
}

int _ww_get_opt(char *argv[], int argc, uint16_t *_modes)
{
	printf(MAG"[   DEV   ]\n"RESET);
	fflush(NULL);
	_ww_help(argv[0], STDOUT_FILENO);

	for (int i = 1; i < argc; i++) {
		if (_ww_is_long_opt(*(argv + i))) {
			process_long_opt(*(argv + i), _modes, *argv);
			argv[i][0] = '\0';
		}
		else if (_ww_is_short_opt(*(argv + i))) {
			process_short_opt(*(argv + i), _modes, *argv);
			argv[i][0] = '\0';
		}
		if (_ww_is_option_set(_modes, _WW_MODE_ERROR))
			return _WW_ERROR;
	}

	// // ----------------------For testing----------------------------
	// // Region from the source file to encrypt
	// *_modes |= _WW_CYPTREG_PHDR,			// Select region using segments
	// // *_modes |= _WW_CYPTREG_PHTEXTX;		// .text segments with x rights
	// // *_modes |= _WW_CYPTREG_PHTEXT,		// .text segments
	// *_modes |= _WW_CYPTREG_PHALL,			// All segments
	// // *_modes |= _WW_CYPTREG_SHDR, 			// Select region using sections
	// // *_modes |= _WW_CYPTREG_SHTEXT,	 	// text section
	// // *_modes |= _WW_CYPTREG_SHDATA,	 	// data section
	// // *_modes |= _WW_CYPTREG_SHALL			// text + data sections

	// // Unpacker and stub insertion region in the target file
	// *_modes |= _WW_INJTREG_PAD; 			// Insert inside segments paddings
	// // *_modes |= _WW_INJTREG_SFT,			// Insert at 0x0 and shift the rest
	// // *_modes |= _WW_INJTREG_END			// Insert at the end of the file
	// // --------------------------------------------------------------
	printf(MAG"[ END DEV ]\n"RESET);
	return 0;
}
