#include "ww.h"

unsigned char *_mapped_data;
off_t       _file_size;
uint16_t    _modes;

void _ww_help(char *progname, int fd) {
	_ww_dprintf(fd, "Usage: %s: [option(s)] [file(s)]\n", progname);
	_ww_dprintf(fd, "Pack [files(s)] by encrypting sections or segments (a.out by default).\n");
	_ww_dprintf(fd, " The options are: \n");
	_ww_dprintf(fd, "  -r\e[4mlevel\e[0m,				Select the region to encrypt\n");
	// TODO: Explain for each level what will be encrypted here
	_ww_dprintf(fd, "  -p,		--padding		Select the padding injection method\n");
	_ww_dprintf(fd, "  -s,		--shifting		Select the shifting injection method\n");
	// TODO: Maybe be more clear about the code injection methods (?)
	_ww_dprintf(fd, "  -h,		--help			Display this information\n");
}

void _ww_display_modes() {
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: _WW_CYPTREG_PHDR (this is maybe not needed).\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHTEXTX))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: .text segments with x rights.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHTEXT))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: .text segments.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHALL))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: all the segments.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHDR))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: _WW_CYPTREG_SHDR (this is maybe not needed).\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHTEXT))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: text section.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHDATA))
		_ww_dprintf(STDOUT_FILENO, "Region to encrypt: data section.\n");
	else if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHALL))
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

int main(int argc, char *argv[])
{
	// if (argc != 2)
	// 	return _ww_print_errors(_WW_ERR_BADARGNBR);

	if (_ww_get_opt(argv, argc, &_modes) == _WW_ERROR) {
		_ww_help(argv[0], STDOUT_FILENO);
		return 1;
	}

	if (_ww_is_option_set(&_modes, _WW_HELP)) {
		_ww_help(argv[0], STDOUT_FILENO);
		return 0;
	}
	_ww_display_modes();
	return 42;

	if (_ww_map_file_into_memory(argv[1]) == _WW_ERROR)
		return 1;

	_ww_process_mapped_data();

	return _ww_write_processed_data_to_file();
}
