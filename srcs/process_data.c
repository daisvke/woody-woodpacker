#include "ww.h"

Elf64_Shdr *get_section_header(void *f, int idx) {
	return f + (((Elf64_Ehdr *)f)->e_shoff + (idx * ((Elf64_Ehdr *)f)->e_shentsize ));
}


Elf64_Shdr *get_text_section_header() {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)_buffer;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them.
	*/
	Elf64_Shdr *strtab = get_section_header(_buffer, ehdr->e_shstrndx);

	/* We iterate over each section header to find .text section name */
	for (size_t i = 1; i < ehdr->e_shnum; i++) {
		Elf64_Shdr *shdr = get_section_header(_buffer, i);
		char *sh_name = (char *)(_buffer + strtab->sh_offset + shdr->sh_name);
		if (strcmp(sh_name, ".text") == 0)
			return shdr;
	}
	return NULL;
}

void	_ww_process_mapped_data()
{
    char*	_key = _ww_keygen(_WW_KEYCHARSET, _WW_KEYSTRENGTH);
    printf("Generated key: %s\n", _key);
	Elf64_Shdr *txt_shdr = get_text_section_header();
	if (!txt_shdr)
		return;
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer + txt_shdr->sh_offset, txt_shdr->sh_size);
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer + txt_shdr->sh_offset, txt_shdr->sh_size);
    free(_key);
}