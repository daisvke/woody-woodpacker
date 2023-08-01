#include "ww.h"

unsigned char *_mapped_data;
off_t       _file_size;
uint16_t    _modes;

void _ww_help(char *progname, int fd) {
	_ww_dprintf(fd, "Usage: %s [option(s)] <binary>\n", progname);
	_ww_dprintf(fd, "Pack <file> by encrypting sections or segments.\n");
	_ww_dprintf(fd, " The options are: \n");
	_ww_dprintf(fd, "  -r\e[4mlevel\e[0m		Select the region to encrypt.\n");
	_ww_dprintf(fd, "   The levels:\n");
	_ww_dprintf(fd, "    \e[4m0\e[0m: text segments with x rights.\n");
	_ww_dprintf(fd, "    \e[4m1\e[0m: .text segments.\n");
	_ww_dprintf(fd, "    \e[4m2\e[0m: all the segments.\n");
	_ww_dprintf(fd, "    \e[4m3\e[0m: text section.\n");
	_ww_dprintf(fd, "    \e[4m4\e[0m: data section.\n");
	_ww_dprintf(fd, "    \e[4m5\e[0m: text + data sections.\n");
	_ww_dprintf(fd, "  -p,		--padding		Insert the code inside the segments padding.\n");
	_ww_dprintf(fd, "  -s,		--shifting		Insert the code at 0x0 and shift the rest.\n");
	_ww_dprintf(fd, "  -h,		--help			Display this information.\n");
}

void _ww_display_modes() {
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHTEXTX) && _ww_set_options(&_modes, _WW_CYPTREG_PHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: .text segments with x rights.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHTEXT) && _ww_set_options(&_modes, _WW_CYPTREG_PHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: .text segments.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHALL) && _ww_set_options(&_modes, _WW_CYPTREG_PHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: all the segments.\n");

	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHTEXT) && _ww_set_options(&_modes, _WW_CYPTREG_SHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: text section.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHDATA) && _ww_set_options(&_modes, _WW_CYPTREG_SHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: data section.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHALL) && _ww_set_options(&_modes, _WW_CYPTREG_SHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: text + data sections.\n");
	else { // Setting a default region to encrypt
		_modes |= _WW_CYPTREG_PHALL;
		dprintf(STDOUT_FILENO, "No region to encrypt selected, default to all the segments.\n");
	}

	if (_ww_is_option_set(&_modes, _WW_INJTREG_PAD))
		_ww_dprintf(STDOUT_FILENO, "Code injection method: insert inside segments paddings.\n");
	else if (_ww_is_option_set(&_modes, _WW_INJTREG_SFT))
		_ww_dprintf(STDOUT_FILENO, "Code injection method: insert at 0x0 and shift the rest.\n");
	else { // Setting a default code injection method
		_modes |= _WW_INJTREG_PAD;
		_ww_dprintf(STDOUT_FILENO, "No code injection method selected, default to inserting inside segments paddings.\n");
	}
}

char *_ww_get_bin_name(char **argv, int argc) {
    char * bin_name = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '\0' && bin_name == NULL)
            bin_name = argv[i];
        else if (argv[i][0] != '\0') {
            _ww_dprintf(STDOUT_FILENO, RED"You must provide only one binary.\n"RESET);
            return NULL;
        }
    }
    return bin_name;
}

int main(int argc, char *argv[])
{
	// After being processed by _ww_get_opt, all the options in
	// the argv array are empty string
	if (_ww_get_opt(argv, argc, &_modes) == _WW_ERROR) {
		_ww_help(argv[0], STDOUT_FILENO);
		return 1;
	}

	if (_ww_is_option_set(&_modes, _WW_HELP)) {
		_ww_help(argv[0], STDOUT_FILENO);
		return 0;
	}
	_ww_display_modes();

	char *binary_name = _ww_get_bin_name(argv, argc);
	if (binary_name == NULL)
		return 1;

	if (_ww_map_file_into_memory(binary_name) == _WW_ERROR)
		return 1;

	_ww_process_mapped_data();

	return _ww_write_processed_data_to_file();
}
