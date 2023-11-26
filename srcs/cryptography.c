#include "ww.h"

/* Generates an encryption key randomly.
	- charset: a set of characters that can be used for the key
	- strength: the width of the key
*/
char*	ww_keygen(const char* charset, size_t strength)
{
    char* 	key = malloc((strength + 1) * sizeof(char));
	if (key == NULL) ww_print_error_and_exit(WW_ERR_ALLOCMEM);
    int		charset_length = ww_strlen(charset);

	// Seed the random number generator according to the current time.
    srand(time(NULL));

    for (size_t i = 0; i < strength; ++i) {
        int	_random_index = rand() % charset_length;
		// Pick a random position from the charset
        key[i] = charset[_random_index];
    }
    key[strength] = '\0'; // null-terminate the key

    return key;
}