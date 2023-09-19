#ifndef _WW_WOODY_WOODPACKER_H
#define _WW_WOODY_WOODPACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>	// For mapping
#include <elf.h>		// For ELF header
#include <string.h>
#include <time.h> 		// For rand & srand

#include "errors.h"

/*-------------------------------------------------------*/

/*  Colors */
#define _WW_RED_COLOR		"\033[31m"
#define _WW_GREEN_COLOR		"\033[32m"
#define _WW_YELLOW_COLOR	"\033[33m"
#define _WW_RESET_COLOR		"\033[0m"

#define _WW_PACKED_FILENAME "woody"
#define _WW_ERROR			1
#define _WW_PAGE_SIZE		4096

/* Encryption */
// Charset used for the encryption key
#define _WW_KEYCHARSET "abcdefghijklmnopqrstuvwxyz" \
					   "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
					   "0123456789"
// Width of the key
#define _WW_KEYSTRENGTH 32

enum _ww_e_modes
{
	// Display detailed notifications
	_WW_VERBOSE = 1,
	// Insert stub inside executable segment's padding
	_WW_INJECTREG_PADDING = 2
};

typedef struct _ww_s_patch
{
	Elf64_Off	_main_entry_offset_from_stub;	// main of the original file
	Elf64_Off	_text_segment_offset_from_stub;	// .text segment offset
	Elf64_Off	_text_length;					// .text section size
} _ww_t_patch;

/*-------------------------------------------------------*/
// Global variables
extern unsigned char	*_mapped_data;
extern Elf64_Off		_file_size;
extern uint16_t			_modes;
/*-------------------------------------------------------*/

size_t		_ww_strlen(const char *s);
void		*_ww_memset(void *src, int c, size_t n);
void		*_ww_memcpy(void *dest, const void *src, size_t n);
int			_ww_strncmp(const char *s1, const char *s2, size_t n);
int			_ww_parse_argv(char *argv[]);
Elf64_Shdr *_ww_get_text_section_header(void);
void		_ww_map_file_into_memory(const char *filename);
void		_ww_process_mapped_data(void);
void		_ww_write_processed_data_to_file(void);
void		_ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header, char *_key);		
void		xor_encrypt_decrypt(void *key, size_t key_length, void *data, size_t data_length);
char		*_ww_keygen(const char *_charset, size_t _strength);

#endif