#include "ww.h"

static void	ww_process_segments(Elf64_Ehdr *elf_header, char *key)
{
	Elf64_Phdr *program_header = (Elf64_Phdr *)(g_mapped_data + elf_header->e_phoff);

	for (size_t i = 0; i < elf_header->e_phnum; i++)
	{
		// Check if the segment contains the .text section
		if (program_header[i].p_type == PT_LOAD && // If phdr is loadable
			(program_header[i].p_flags & PF_X))    // If phdr is executable
		{
			// If .text, then proceed to injection, otherwise return
			ww_inject_stub(elf_header, &program_header[i], key);
			return;
		}
	}
}

Elf64_Shdr	*get_section_header(void *f, int _idx)
{
	return f + (((Elf64_Ehdr *)f)->e_shoff + (_idx * ((Elf64_Ehdr *)f)->e_shentsize));
}

Elf64_Shdr	*ww_get_text_section_header()
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)g_mapped_data;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them
	 */
	Elf64_Shdr *strtab = get_section_header(g_mapped_data, ehdr->e_shstrndx);

	// We iterate over each section header to find .text section name
	for (size_t i = 1; i < ehdr->e_shnum; i++)
	{
		Elf64_Shdr *shdr = get_section_header(g_mapped_data, i);
		char *sh_name = (char *)(g_mapped_data + strtab->sh_offset + shdr->sh_name);
		if (ww_strncmp(sh_name, ".text", 5) == 0)
			return shdr;
	}
	return NULL;
}

static void	ww_checkelf_header_integrity_and_exit_on_error(Elf64_Ehdr *elf_header)
{
	if (elf_header->e_ehsize != sizeof(Elf64_Ehdr) ||
		elf_header->e_phentsize != sizeof(Elf64_Phdr) ||
		elf_header->e_shentsize != sizeof(Elf64_Shdr) ||
		(elf_header->e_type != ET_EXEC && elf_header->e_type != ET_DYN) ||
		((elf_header->e_phoff +
			(elf_header->e_phnum * elf_header->e_phentsize)) > g_file_size) ||
		((elf_header->e_shoff +
			(elf_header->e_shnum * elf_header->e_shentsize)) > g_file_size)
	)
	ww_print_error_and_exit(WW_ERR_CORRUPTEHDR);
}

/* Encrypt .text section before injection
 * as we want the final output file to have its main code obfuscated
 */
void	ww_process_mapped_data()
{
	Elf64_Ehdr	*elf_header = (Elf64_Ehdr *)g_mapped_data;
	ww_checkelf_header_integrity_and_exit_on_error(elf_header);
	Elf64_Shdr	*txt_shdr = ww_get_text_section_header();
	if (!txt_shdr) ww_print_error_and_exit(WW_ERR_NOTEXTSEC);

	printf("\n" WW_YELLOW_BG " > STARTING ENCRYPTION OF THE .TEXT SECTION... < "
		WW_RESET_BG_COLOR "\n");
	// Generate the key that will be used for the encryption
	char *key = ww_keygen(WW_KEYCHARSET, WW_KEYSTRENGTH);
	printf("Generated random key => " WW_YELLOW_COLOR "%s\n", key);

	/* Encrypt the .text section before inserting the parasite code.
	 * The section will be decrypted by the latter during execution.
	 */
	xor_with_additive_cipher(
		key,
		WW_KEYSTRENGTH,
		g_mapped_data + txt_shdr->sh_offset,
		txt_shdr->sh_size,
		0 // Encrypt mode
	);
	printf(WW_GREEN_COLOR "Done!\n\n" WW_RESET_COLOR);

	printf(WW_YELLOW_BG " > STARTING PARASITE INJECTION... < "
		WW_RESET_BG_COLOR "\n");
	ww_process_segments(elf_header, key);
}
