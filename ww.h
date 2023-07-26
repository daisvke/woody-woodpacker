#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdint.h>
# include <sys/mman.h> // For mapping
# include <elf.h>	  // For ELF header
# include <string.h>
# include <time.h> // For rand & srand
# include <stdbool.h> // For bool
 
# include "errors.h"
# include "colorcodes.h"

/*-------------------------------------------------------*/

/*  Colors */
# define _WW_RED_COLOR		"\033[31m"
# define _WW_GREEN_COLOR	"\033[32m"
# define _WW_YELLOW_COLOR	"\033[33m"
# define _WW_RESET_COLOR	"\033[0m"

/* Return */
# define _WW_ERROR	1

/* Encryption */
// Charset used for the encryption key
# define _WW_KEYCHARSET	"abcdefghijklmnopqrstuvwxyz" \
						"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
						"0123456789"
// Width of the key
# define _WW_KEYSTRENGTH	18

// Region from the source file to encrypt
enum	_ww_e_crypt_region
{
	_WW_CYPTREG_PHDR = 1,	 // Select region using segments
	_WW_CYPTREG_PHTEXTX = 2, // .text segments with x rights
	_WW_CYPTREG_PHTEXT = 4,	 // .text segments
	_WW_CYPTREG_PHALL = 8,	 // All segments
	_WW_CYPTREG_SHDR = 16,	 // Select region using sections
	_WW_CYPTREG_SHTEXT = 32, // text section
	_WW_CYPTREG_SHDATA = 64, // data section
	_WW_CYPTREG_SHALL = 128	 // text + data sections

	// Unpacker and stub insertion region in the target file
};

// Unpacker and stub injection region in the target file
enum	_ww_e_inject_region
{
	_WW_INJTREG_PAD = 256, // Insert inside segments paddings
	_WW_INJTREG_SFT = 512, // Insert at 0x0 and shift the rest
	_WW_INJTREG_END = 1024 // Insert at the end of the file
};

# define _WW_HELP 2048 		 // If the help option was chosen
# define _WW_MODE_ERROR 4096 // If an incorrect option was chosen

// Struct that hold options, it's used as an array
typedef struct {
    const char short_opt;
    const char* long_opt;
} _ww_option;

/*-------------------------------------------------------*/
// Global variables
extern unsigned char	*_mapped_data;
extern off_t			_file_size;
extern uint16_t			_modes;
/*-------------------------------------------------------*/

size_t	_ww_strlen(const char *s);

int		_ww_print_errors(enum _ww_e_errors err_code);

int		_ww_get_opt(char *argv[], int argc, uint16_t *_modes);

int		_ww_map_file_into_memory(const char *filename);
void	_ww_process_mapped_data();
int		_ww_write_processed_data_to_file(void);
void	_ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header);

void	xor_encrypt_decrypt(void *key, size_t key_length, void *data, size_t data_length);
char	*_ww_keygen(const char *_charset, size_t _strength);

#endif
