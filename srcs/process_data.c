#include "ww.h"

static void	_ww_process_segments(Elf64_Ehdr *_elf_header, char *_key)
{
	Elf64_Phdr *_program_header = (Elf64_Phdr *)(_mapped_data + _elf_header->e_phoff);

	for (size_t i = 0; i < _elf_header->e_phnum; i++)
	{
		// Check if the segment contains the .text section
		if (_program_header[i].p_type == PT_LOAD && // If phdr is loadable
			(_program_header[i].p_flags & PF_X))    // If phdr is executable
		{
			// If .text, then proceed to injection, otherwise return
			_ww_inject_stub(_elf_header, &_program_header[i], _key);
			return;
		}
	}
}

Elf64_Shdr	*get_section_header(void *_f, int _idx)
{
	return _f + (((Elf64_Ehdr *)_f)->e_shoff + (_idx * ((Elf64_Ehdr *)_f)->e_shentsize));
}

Elf64_Shdr	*_ww_get_text_section_header()
{
	Elf64_Ehdr *_ehdr = (Elf64_Ehdr *)_mapped_data;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them
	 */
	Elf64_Shdr *_strtab = get_section_header(_mapped_data, _ehdr->e_shstrndx);

	// We iterate over each section header to find .text section name
	for (size_t _i = 1; _i < _ehdr->e_shnum; _i++)
	{
		Elf64_Shdr *_shdr = get_section_header(_mapped_data, _i);
		char *_sh_name = (char *)(_mapped_data + _strtab->sh_offset + _shdr->sh_name);
		if (_ww_strncmp(_sh_name, ".text", 5) == 0)
			return _shdr;
	}
	return NULL;
}

static void	_ww_check_elf_header_integrity_and_exit_on_error(Elf64_Ehdr *_elf_header)
{
	if (_elf_header->e_ehsize != sizeof(Elf64_Ehdr) ||
		_elf_header->e_phentsize != sizeof(Elf64_Phdr) ||
		_elf_header->e_shentsize != sizeof(Elf64_Shdr) ||
		(_elf_header->e_type != ET_EXEC && _elf_header->e_type != ET_DYN) ||
		((_elf_header->e_phoff +
			(_elf_header->e_phnum * _elf_header->e_phentsize)) > _file_size) ||
		((_elf_header->e_shoff +
			(_elf_header->e_shnum * _elf_header->e_shentsize)) > _file_size)
	)
		_ww_print_error_and_exit(_WW_ERR_CORRUPTEHDR);
}

/* Encrypt .text section before injection
 * as we want the final output file to have its main code obfuscated
 */
void	_ww_process_mapped_data()
{
	Elf64_Ehdr	*_elf_header = (Elf64_Ehdr *)_mapped_data;
	_ww_check_elf_header_integrity_and_exit_on_error(_elf_header);
	Elf64_Shdr	*txt_shdr = _ww_get_text_section_header();
	if (!txt_shdr) _ww_print_error_and_exit(_WW_ERR_NOTEXTSEC);

	printf("\n" _WW_YELLOW_BG " > STARTING ENCRYPTION OF THE .TEXT SECTION... < "
		_WW_RESET_BG_COLOR "\n");
	// Generate the key that will be used for the encryption
	char *_key = _ww_keygen(_WW_KEYCHARSET, _WW_KEYSTRENGTH);
	printf("Generated random key => " _WW_YELLOW_COLOR "%s\n", _key);

	/* Encrypt the .text section before inserting the parasite code.
	 * The section will be decrypted by the latter during execution.
	 */
	xor_encrypt_decrypt(
		_key,
		_WW_KEYSTRENGTH,
		_mapped_data + txt_shdr->sh_offset,
		txt_shdr->sh_size
	);
	printf(_WW_GREEN_COLOR "Done!\n\n" _WW_RESET_COLOR);

	printf(_WW_YELLOW_BG " > STARTING PARASITE INJECTION... < "
		_WW_RESET_BG_COLOR "\n");
	_ww_process_segments(_elf_header, _key);
}