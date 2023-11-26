#ifndef WW_WOODY_WOODPACKER_H
#define WW_WOODY_WOODPACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>   // For mapping
#include <elf.h>        // For ELF header
#include <string.h>
#include <time.h>       // For rand & srand
#include "errors.h"

/* Text colors */
#define WW_RED_COLOR       "\033[31m"
#define WW_GREEN_COLOR     "\033[32m"
#define WW_YELLOW_COLOR    "\033[33m"
#define WW_RESET_COLOR     "\033[0m"
/* Background colors */
#define WW_GREEN_BG        "\033[42m"
#define WW_YELLOW_BG       "\033[43m"
#define WW_RESET_BG_COLOR  "\033[49m"

#define WW_PACKED_FILENAME "woody"
#define WW_ERROR           1
#define WW_KEYSTRENGTH     32
// Common value representing the size of a memory page
// in many computer systems
#define WW_PAGE_SIZE       4096

// Charset used for the encryption key
#define WW_KEYCHARSET      "abcdefghijklmnopqrstuvwxyz" \
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                            "0123456789"
enum ww_eg_modes
{
    // Display detailed notifications
    WW_VERBOSE = 1,
    // Insert stub inside executable segment's padding
    WW_INJECTREG_PADDING = 2,
    // Insert at the end of the .text segment
    // then shift all the data coming after
    WW_INJECTREG_SHIFT = 3
};

typedef struct ww_s_patch
{
    Elf64_Off   main_entry_offset_from_stub;   // main of the original file
    Elf64_Off   text_segment_offset_from_stub; // .text segment offset
    Elf64_Off   text_section_offset_from_stub; // .text section offset
    Elf64_Off   text_length;                   // .text section size
}   ww_t_patch;

/*-------------------------------------------------------*/
// Global variables
extern unsigned char    *g_mapped_data;  // file is mapped in memory here
extern Elf64_Off        g_file_size;     // size of the mapped file
extern uint16_t         g_modes;         // options given from command line
/*-------------------------------------------------------*/

size_t      ww_strlen(const char *s);
void        *ww_memset(void *src, int c, size_t n);
void        *ww_memcpy(void *dest, const void *src, size_t n);
int         ww_strncmp(const char *s1, const char *s2, size_t n);
int         ww_parse_argv(char *argv[]);
Elf64_Shdr  *ww_get_text_section_header(void);
void        ww_map_file_into_memory(const char *filename);
void        ww_process_mapped_data(void);
void        ww_write_processed_data_to_file(void);
void        ww_inject_stub( \
    Elf64_Ehdr *elf_header, Elf64_Phdr *program_header, char *key);
void        xor_encrypt_decrypt( \
    void *key, size_t key_length, void *data, size_t data_length);
char        *ww_keygen(const char *_charset, size_t strength);

#endif