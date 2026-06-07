#include "ww.h"

int ww_get_executable_segment_index(Elf64_Ehdr *elf_header, Elf64_Phdr *program_header)
{
	for (size_t i = 0; i < elf_header->e_phnum; i++)
		// Check if the segment contains the .text section
		if (program_header[i].p_type == PT_LOAD &&	// If phdr is loadable
			(program_header[i].p_flags & PF_X) &&	// If phdr is executable
			(program_header[i].p_flags & PF_R))		// If phdr is readable
			return i;
	return -1;
}

/*
 * The function returns the closest PT_LOAD segment whose p_offset
 * is strictly greater than the injection point.
 *
 * If multiple segments satisfy this condition, the one with the smallest
 * p_offset is selected (i.e. the immediate next segment in file layout).
 */
Elf64_Phdr *ww_get_next_load_segment_by_offset(
    Elf64_Ehdr *elf_header,
    Elf64_Phdr *program_headers,
    Elf64_Off injection_offset)
{
    Elf64_Phdr *closest = NULL;

    // Iterate over all program headers in the ELF binary
    for (size_t i = 0; i < elf_header->e_phnum; i++)
    {
        // Only consider LOAD segments (mapped into memory at runtime)
        if (program_headers[i].p_type != PT_LOAD)
            continue;

        /*
         * We only care about segments that come after the injection point
         * in the file layout.
         */
        if (program_headers[i].p_offset > injection_offset)
        {
            /*
             * Select the closest segment after the injection point.
             * This ensures we find the immediate next boundary, not a distant one.
             */
            if (!closest || program_headers[i].p_offset < closest->p_offset)
                closest = &program_headers[i];
        }
    }

    return closest;
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

/* Encrypt .text section before injection as we want the final output file
 *  to have its main code obfuscated in case we are packing a virus
 */
void	ww_process_mapped_data()
{
	Elf64_Ehdr	*elf_header = (Elf64_Ehdr *)g_mapped_data;
	// Check if the program_header is not corrupted
	ww_checkelf_header_integrity_and_exit_on_error(elf_header);
	// Retrieve the text section header
	Elf64_Shdr	*txt_shdr = ww_get_text_section_header();
	if (!txt_shdr) ww_print_error_and_exit(WW_ERR_NOTEXTSEC);

	printf(WW_YELLOW_COLOR "\n▶ STARTING ENCRYPTION OF THE .TEXT SECTION...\n\n" WW_RESET_COLOR);

	// Generate the key that will be used for the encryption
	char *key = ww_keygen(WW_KEYCHARSET, WW_KEYSTRENGTH);
	printf(WW_YELLOW_COLOR "▶ GENERATED RANDOM KEY\n" WW_RESET_COLOR);
	printf("   %s\n", key);

	/* Encrypt the .text section before inserting the parasite code.
	 * The section will be decrypted by the latter during execution.
	 */
	xor_with_additive_cipher(
		key,			// The randomly generated encryption key
		WW_KEYSTRENGTH,	// The key width
		// Get the .text section offset to start encrypting there 
		g_mapped_data + txt_shdr->sh_offset,
		txt_shdr->sh_size, // The .text section size
		0 // Encrypt mode
	);

	printf(WW_YELLOW_COLOR "\n▶ STARTING SHELLCODE INJECTION...\n\n" WW_RESET_COLOR);

	Elf64_Phdr	*program_header = (Elf64_Phdr *)(g_mapped_data + elf_header->e_phoff);
	int			executable_segment_index = ww_get_executable_segment_index(elf_header, program_header);

	if (executable_segment_index == -1)
		ww_print_error_and_exit(WW_ERR_EXEC_SEGMENT_NOT_FOUND);

	ww_inject_stub(
		elf_header, &program_header[executable_segment_index], key
	);
}
