#include "ww.h"

static void _ww_process_segments(Elf64_Ehdr *_elf_header, char *_key)
{
	Elf64_Phdr *_program_header = (Elf64_Phdr *)(_mapped_data + _elf_header->e_phoff);

	printf("\n");
	for (size_t i = 0; i < _elf_header->e_phnum; i++)
	{
		// Check if the segment is for the "text" section
		printf("> SEGMENT %ld -> type: %d\n", i, _program_header[i].p_type);
		printf("-----------------------------------------------\n");
		if (_program_header[i].p_type == PT_LOAD && // If phdr is loadable
			(_program_header[i].p_flags & PF_X))	// If phdr is executable
		{
			_ww_inject_stub(_elf_header, &_program_header[i]);
			return;
		}
	}
}

Elf64_Shdr *get_section_header(void *f, int idx)
{
	return f + (((Elf64_Ehdr *)f)->e_shoff + (idx * ((Elf64_Ehdr *)f)->e_shentsize));
}

Elf64_Shdr *get_text_section_header()
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)_mapped_data;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them.
	 */
	Elf64_Shdr *strtab = get_section_header(_mapped_data, ehdr->e_shstrndx);

	/* We iterate over each section header to find .text section name */
	for (size_t i = 1; i < ehdr->e_shnum; i++)
	{
		Elf64_Shdr *shdr = get_section_header(_mapped_data, i);
		char *sh_name = (char *)(_mapped_data + strtab->sh_offset + shdr->sh_name);
		if (strcmp(sh_name, ".text") == 0) // TODO replace strcmp
			return shdr;
	}
	return NULL;
}

void _ww_process_mapped_data()
{
	// char*	_key = _ww_keygen(_WW_KEYCHARSET, _WW_KEYSTRENGTH);
	char *_key = "abcdefghijklmnopqr\0";
	printf("Generated key: %s\n", _key);

	Elf64_Ehdr *_elf_header = (Elf64_Ehdr *)_mapped_data;

	printf("Starting .text section enryption...\n");
	Elf64_Shdr *txt_shdr = get_text_section_header();
	if (!txt_shdr)
		_ww_print_error_and_exit(_WW_ERR_NOTEXTSEC);
	//
	xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + txt_shdr->sh_offset, txt_shdr->sh_size);
	xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + txt_shdr->sh_offset, txt_shdr->sh_size);

	printf("\nStarting parasite injection...\n");
	_ww_process_segments(_elf_header, _key);

	// free(_key);
}