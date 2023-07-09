#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdint.h>
# include <sys/mman.h>		// For mapping 
# include <elf.h>			// For ELF header
# include <string.h>
# include <time.h>			// For rand & srand

# include "errors.h"

/*-------------------------------------------------------*/

/*  Colors */
# define _WW_RED_COLOR		"\033[31m"
# define _WW_GREEN_COLOR	"\033[32m"
# define _WW_YELLOW_COLOR	"\033[33m"
# define _WW_RESET_COLOR	"\033[0m"

/* Return */
# define _WW_ERROR		1

/* String header for output file
	This header is an indication that the file has
	been encrypted by this program
*/
# define _WW_PROG_HEADER	"....WOODY....\n"

/* Encryption */
// Charset used for the encryption key
# define _WW_KEYCHARSET		"abcdefghijklmnopqrstuvwxyz" \
							"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
							"0123456789"
// Width of the key
# define _WW_KEYSTRENGTH	18

enum	_ww_e_encrypt_target
{
	_WW_CRYPTARG_PHTEXTX,	// .text segments with x rights
	_WW_CRYPTARG_PHTEXTXALL,// .text with x rights + some other segments
	_WW_CRYPTARG_PHALL,		// All segments
	_WW_CRYPTARG_SHTEXT,	// text section
	_WW_CRYPTARG_SHDATA		// data section
	_WW_CRYPTARG_SHALL		// text + data sections
};

/*-------------------------------------------------------*/
// Global variables
extern unsigned char*	_mapped_data;
extern off_t	        _file_size;
/*-------------------------------------------------------*/

size_t	_ww_strlen(const char *s);

int		_ww_print_errors(enum _ww_e_errors err_code);

int		_ww_map_file_into_memory(const char *filename);
void	_ww_process_mapped_data();
int		_ww_write_processed_data_to_file(void);

void	xor_encrypt_decrypt(void *key, size_t key_length, void *data, size_t data_length);
char*	_ww_keygen(const char* _charset, size_t _strength);

#endif
