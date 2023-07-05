#include "ww.h"

/* Generates an encryption key randomly.
	- charset: a set of characters that can be used for the key
	- strength: the width of the key
*/
char*	_ww_keygen(const char* _charset, size_t _strength)
{
    char* 	_key = malloc((_strength + 1) * sizeof(char));
	if (_key == NULL) _ww_print_errors(_WW_ERR_ALLOCMEM);
    int		_charset_length = _ww_strlen(_charset);

	// Seed the random number generator according to the current time.
    srand(time(NULL));

    for (size_t i = 0; i < _strength; ++i) {
        int	_random_index = rand() % _charset_length;
		// Pick a random position from the charset
        _key[i] = _charset[_random_index];
    }
    _key[_strength] = '\0'; // null-terminate the key

    return _key;
}