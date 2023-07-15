#include "ww.h"
#include "stub.h"

// For testing
void elf_print_header(Elf64_Ehdr *data)
{
	printf("Entry point : %ld\n", data->e_entry);
	printf("Segment table address (relative) : + %4ld bytes\n", data->e_phoff);
	printf("Number of entries in the segment table : %d\n", data->e_phnum);
	printf("Section table address (relative) : + %4ld bytes\n", data->e_shoff);
	printf("Number of entries in the section table : %d\n\n", data->e_shnum);
}

// For testing
void elf_print_sections_name(Elf64_Ehdr *data)
{
	int i;

	Elf64_Shdr *section_header_start = (Elf64_Shdr *)((void *)data + data->e_shoff);
	Elf64_Shdr sections_string = section_header_start[data->e_shstrndx];
	char *strings = (char *)((void *)data + sections_string.sh_offset);
	Elf64_Shdr section_header;

	for (i = 0; i < data->e_shnum; i++)
	{
		section_header = section_header_start[i];
		printf("%s\n", strings + section_header.sh_name);
	}
	printf("\n");
	
}

static int	_ww_authorize_encryption(int _type, int _flags)
{
		// Desactivate program header encryption
	if ((_type != PT_PHDR && _type != PT_INTERP) &&
		// Desactivate encryption of non executable .text segment (causes segfault)
		!(_type == PT_LOAD && !(_flags & PF_X)) &&
		(
			(_modes & _WW_CYPTREG_PHALL ) ||
			((_modes & _WW_CYPTREG_PHTEXTX) && (_type == PT_LOAD && (_flags & PF_X))) ||
			((_modes & _WW_CYPTREG_PHTEXT) && (_type == PT_LOAD))
		))
		return 1;
	return 0;
}

static void _ww_encrypt_segments(Elf64_Ehdr *_elf_header, char *_key)
{
	// Iterate over the program headers
	Elf64_Phdr *_program_header = (Elf64_Phdr *)(_mapped_data + _elf_header->e_phoff);
	off_t	_injection_offset = 0;

	for (size_t i = 0; i < _elf_header->e_phnum; i++)
	{
		// Check if the segment is for the "text" section
		printf("-----------------------------------------------\n");
		printf("* SEGMENT %ld -> type: %d\n", i, _program_header->p_type);
		if (_ww_authorize_encryption(
				_program_header->p_type,
				_program_header->p_flags
			))
		{
			printf("Segment address: 0x%lx\n", (unsigned long)_program_header->p_vaddr);
			printf("Segment size: %lu bytes\n\n", (unsigned long)_program_header->p_filesz);
			// Only crypt if memory size is not null
			if (_program_header->p_memsz > 0)
			{
				// We use the p_filesz field and not the p_memsz field since
				// the latter represents the size of the segment in memory,
				// which may include additional padding or allocated memory that
				// we do not need to encrypt here.
				xor_encrypt_decrypt(
					_key, _WW_KEYSTRENGTH,
					_mapped_data + _program_header->p_offset,
					_program_header->p_filesz
				);
			}
			// If segment padding injectin mode is on.
			if (_modes & _WW_INJTREG_PAD)
			{
				if (_program_header->p_type == PT_LOAD &&
					(_program_header->p_flags & PF_X))
				{
					_injection_offset =
						_program_header->p_offset + _program_header->p_filesz;
					off_t	_injection_addr = _program_header->p_vaddr + _program_header->p_filesz;
					printf(_WW_YELLOW_COLOR "\n\nInjection start offset: %lx\n", _injection_offset);
					// Getting next segment's header
					Elf64_Phdr *_next_program_header = 
						(Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);
					off_t	_padding_size = _next_program_header->p_vaddr - _injection_addr;
					printf("Next segment offset: %lx\n", _next_program_header->p_offset);
					printf("Padding size: %ld\n", _padding_size);

					printf("code size: %ld\n", sizeof(_stub));
					_ww_memcpy(_mapped_data + _injection_offset, _stub, sizeof(_stub));

					Elf64_Ehdr	*_elf_header = (Elf64_Ehdr *)_mapped_data;
					_elf_header->e_entry = _program_header->p_vaddr + _program_header->p_filesz;
					printf("e_entry address: %lx\n", _elf_header->e_entry);
					_program_header->p_filesz += sizeof(_stub);
					_program_header->p_memsz += sizeof(_stub);
					printf("NEW Segment size: %lu bytes\n\n" _WW_RESET_COLOR, (unsigned long)_program_header->p_filesz);
				}
			}
			// elf_print_sections_name(_elf_header);
		}
		_program_header =
			(Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);
		printf("\n");
	}
}

Elf64_Shdr *get_section_header(void *f, int idx) {
	return f + (((Elf64_Ehdr *)f)->e_shoff + (idx * ((Elf64_Ehdr *)f)->e_shentsize ));
}

Elf64_Shdr *get_text_section_header() {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)_mapped_data;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them.
	*/
	Elf64_Shdr *strtab = get_section_header(_mapped_data, ehdr->e_shstrndx);
  
	/* We iterate over each section header to find .text section name */
	for (size_t i = 1; i < ehdr->e_shnum; i++) {
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

	// For testing
	// elf_print_header(elf_header);
	elf_print_sections_name(_elf_header);

	// If selecting encryption region according to segments
	if (_modes & _WW_CYPTREG_PHDR)
	{
		printf("Starting segment enryption...\n");
		_ww_encrypt_segments(_elf_header, _key);
	} 
	else // If selecting encryption region according to sections
	{
		printf("Starting section enryption...\n");

		Elf64_Shdr *txt_shdr = get_text_section_header();
		if (!txt_shdr)
			return;
		xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + txt_shdr->sh_offset, txt_shdr->sh_size);
		xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + txt_shdr->sh_offset, txt_shdr->sh_size);
	}
	// To encrypt everything from program header (excluded) to section header (excluded)
	// size_t ph_size = elf_header->e_phentsize * elf_header->e_phnum;
	// xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + ph_size + elf_header->e_ehsize, _file_size - ph_size - elf_header->e_ehsize - elf_header->e_shentsize);
	
	// xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _mapped_data + elf_header->e_shoff, _file_size);

	// free(_key);
}