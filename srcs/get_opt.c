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

int _ww_set_options(uint16_t *options, uint16_t opt_flag) {
	*options |= opt_flag;
    return 1;
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
						_ww_set_options(_modes, _WW_MODE_ERROR);
					}
					else {
						if (arg[i] == '0')
							// .text segments with x rights
							opt_flag = _WW_CYPTREG_PHTEXTX;
						else if (arg[i] == '1')
							// .text segments
							opt_flag = _WW_CYPTREG_PHTEXT;
						else if (arg[i] == '2')
							// All segments
							opt_flag = _WW_CYPTREG_PHALL;
						else if (arg[i] == '3')
							// text section
							opt_flag = _WW_CYPTREG_SHTEXT;
						else if (arg[i] == '4')
							// data section
							opt_flag = _WW_CYPTREG_SHDATA;
						else if (arg[i] == '5')
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
				_ww_set_options(_modes, opt_flag);
				break;
			}
			else if (j + 1 == count) {
				_ww_set_options(_modes, _WW_MODE_ERROR); // option_error
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
			_ww_set_options(_modes, opt_flag);
			return;
		}
	}
	_ww_set_options(_modes, _WW_MODE_ERROR);
	_ww_dprintf(STDERR_FILENO, "%s: unrecognized option '%s'\n", progname, arg);
}

int _ww_get_opt(char *argv[], int argc, uint16_t *_modes)
{
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
	return 0;
}
