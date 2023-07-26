#include "ww.h"

unsigned char *_mapped_data;
off_t       _file_size;
uint16_t    _modes;

void what_options() {
	printf(YEL"What options are set: \n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHDR))
		printf("_WW_CYPTREG_PHDR\n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHTEXTX))
		printf("_WW_CYPTREG_PHTEXTX\n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHTEXT))
		printf("_WW_CYPTREG_PHTEXT\n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_PHALL))
		printf("_WW_CYPTREG_PHALL\n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHDR))
		printf("_WW_CYPTREG_SHDR\n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHTEXT))
		printf("_WW_CYPTREG_SHTEXT\n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHDATA))
		printf("_WW_CYPTREG_SHDATA\n");
	if (_ww_is_option_set(&_modes, _WW_CYPTREG_SHALL))
		printf("_WW_CYPTREG_SHALL\n");
	if (_ww_is_option_set(&_modes, _WW_INJTREG_PAD))
		printf("_WW_INJTREG_PAD\n");
	if (_ww_is_option_set(&_modes, _WW_INJTREG_SFT))
		printf("_WW_INJTREG_SFT\n");
	if (_ww_is_option_set(&_modes, _WW_INJTREG_END))
		printf("_WW_INJTREG_END\n");
	if (_ww_is_option_set(&_modes, _WW_HELP))
		printf("_WW_HELP\n");

	printf("\n"RESET);
}

int main(int argc, char *argv[])
{
	// if (argc != 2)
	// 	return _ww_print_errors(_WW_ERR_BADARGNBR);

	if (_ww_get_opt(argv, argc, &_modes) == _WW_ERROR)
		return 1;

	what_options();
	return 42;

	if (_ww_map_file_into_memory(argv[1]) == _WW_ERROR)
		return 1;

	_ww_process_mapped_data();

	return _ww_write_processed_data_to_file();
}
