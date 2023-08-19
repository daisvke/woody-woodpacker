#include "ww.h"

static int _ww_authorize_encryption(int _type, int _flags)
{
	// Desactivate program header encryption
	if ((_type != PT_PHDR && _type != PT_INTERP) &&
		// Desactivate encryption of non executable .text segment (causes segfault)
		!(_type == PT_LOAD && !(_flags & PF_X)) &&
		((_modes & _WW_CRYPTREG_PHALL) ||
		 ((_modes & _WW_CRYPTREG_PHTEXTX) && (_type == PT_LOAD && (_flags & PF_X))) ||
		 ((_modes & _WW_CRYPTREG_PHTEXT) && (_type == PT_LOAD))))
		return 1;
	return 0;
}

static void _ww_process_segments(Elf64_Ehdr *_elf_header, char *_key)
{
	// Iterate over the program headers
	Elf64_Phdr *_program_header = (Elf64_Phdr *)(_mapped_data + _elf_header->e_phoff);

	printf("\n");
	for (size_t i = 0; i < _elf_header->e_phnum; i++)
	{
		// Check if the segment is for the "text" section
		printf("> SEGMENT %ld -> type: %d\n", i, _program_header->p_type);
		printf("-----------------------------------------------\n");
		if (_ww_authorize_encryption(
				_program_header->p_type,
				_program_header->p_flags))
		{
			// Only crypt if memory size is not null
			if (_program_header->p_memsz > 0)
			{
				// We use the p_filesz field and not the p_memsz field since
				// the latter represents the size of the segment in memory,
				// which may include additional padding or allocated memory that
				// we do not need to encrypt here.
				// xor_encrypt_decrypt(
				// 	_key, _WW_KEYSTRENGTH,
				// 	_mapped_data + _program_header->p_offset,
				// 	_program_header->p_filesz
				// );
			}
			_ww_inject_stub(_elf_header, _program_header);
			break;
		}
		_program_header =
			(Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);
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
		if (strcmp(sh_name, ".text") == 0)
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

	// If selecting encryption region according to segments
	if (_modes & _WW_CRYPTREG_PHDR)
	{
		printf("Starting segment enryption...\n");
		_ww_process_segments(_elf_header, _key);
	}
	else // If selecting encryption region according to sections
	{
		printf("Starting section enryption...\n");

		Elf64_Shdr *txt_shdr = get_text_section_header();
		if (!txt_shdr)
			return;
		xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + txt_shdr->sh_offset, txt_shdr->sh_size);
		// xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + txt_shdr->sh_offset, txt_shdr->sh_size);
	}
	// To encrypt everything from program header (excluded) to section header (excluded)
	// size_t ph_size = elf_header->e_phentsize * elf_header->e_phnum;
	// xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + ph_size + elf_header->e_ehsize, _file_size - ph_size - elf_header->e_ehsize - elf_header->e_shentsize);

	// xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + elf_header->e_shoff, _file_size);

	// free(_key);
}