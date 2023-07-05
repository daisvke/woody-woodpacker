#include "ww.h"

void	_ww_process_mapped_data()
{
    char*	_key = _ww_keygen(_WW_KEYCHARSET, _WW_KEYSTRENGTH);
    printf("Generated key: %s\n", _key);
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer, _file_size);
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer, _file_size);
    free(_key);
}