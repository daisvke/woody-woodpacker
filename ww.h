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
 
# include "errors.h"

/*-------------------------------------------------------*/

/*  Colors */
# define _WW_RED_COLOR		"\033[31m"
# define _WW_GREEN_COLOR	"\033[32m"
# define _WW_YELLOW_COLOR	"\033[33m"
# define _WW_RESET_COLOR	"\033[0m"

# define _WW_PACKED_FILENAME	"woody"

/* Return */
# define _WW_ERROR	1

# define _WW_PAGE_SIZE	4096

/* Encryption */
// Charset used for the encryption key
# define _WW_KEYCHARSET		"abcdefghijklmnopqrstuvwxyz" \
							"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
							"0123456789"
// Width of the key
# define _WW_KEYSTRENGTH	18

/*-------------------------------------------------------*/
// Modes

// Region from the source file to encrypt
enum	_ww_e_crypt_region
{
	_WW_CRYPTREG_PHDR = 1,	 // Select region using segments
	_WW_CRYPTREG_PHTEXTX = 2, // .text segments with x rights
	_WW_CRYPTREG_PHTEXT = 4,	 // .text segments
	_WW_CRYPTREG_PHALL = 8,	 // All segments
	_WW_CRYPTREG_SHDR = 16,	 // Select region using sections
	_WW_CRYPTREG_SHTEXT = 32, // text section
	_WW_CRYPTREG_SHDATA = 64, // data section
	_WW_CRYPTREG_SHALL = 128	 // text + data sections
};

// Unpacker and stub injection region in the target file
enum	_ww_e_inject_region
{
	// Insert inside executable segment's padding
	_WW_INJECTREG_PADDING = 256,
};

/*-------------------------------------------------------*/
// Global variables
extern unsigned char	*_mapped_data;
extern off_t			_file_size;
extern uint16_t			_modes;
/*-------------------------------------------------------*/

size_t	_ww_strlen(const char *s);
void    *_ww_memset(void *src, int c, size_t n);
void    *_ww_memcpy(void *dest, const void *src, size_t n);

int		_ww_parse_argv(char *argv[]);

Elf64_Shdr *get_section_header(void *f, int idx);

void	_ww_map_file_into_memory(const char *filename);
void	_ww_process_mapped_data();
void	_ww_write_processed_data_to_file(void);
void	_ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header);

void	xor_encrypt_decrypt(void *key, size_t key_length, void *data, size_t data_length);
char	*_ww_keygen(const char *_charset, size_t _strength);

#endif