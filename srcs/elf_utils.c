#include "ww.h"

Elf64_Shdr	*get_section_header(void *f, int idx) {
	return f + (((Elf64_Ehdr *)f)->e_shoff + (idx * ((Elf64_Ehdr *)f)->e_shentsize ));
}

Elf64_Phdr	*get_program_header(void *f, int idx) {
	return f + (((Elf64_Ehdr *)f)->e_phoff + (idx * ((Elf64_Ehdr *)f)->e_phentsize ));
}

Elf64_Shdr	*get_section_header_by_name(void *f, char *section_name, int *idx) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)f;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them.
	*/
	Elf64_Shdr *strtab = get_section_header(f, ehdr->e_shstrndx);

	/* We iterate over each section header to find .text section name */
	for (size_t i = 1; i < ehdr->e_shnum; i++) {
		Elf64_Shdr *shdr = get_section_header(f, i);
		char *sh_name = (char *)(f + strtab->sh_offset + shdr->sh_name);
		if (strcmp(sh_name, section_name) == 0) {
			*idx = i;
			return shdr;
		}
	}
	return NULL;
}
