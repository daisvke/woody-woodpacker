#include "ww.h"
#include "stub.h"

void _ww_shift_offsets_after_stub_insertion(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header, size_t i)
{
	size_t _stub_size_with_pad = ((sizeof(_stub) /_WW_PAGE_SIZE) + 1) * _WW_PAGE_SIZE;
	off_t _injection_addr = _program_header->p_vaddr + _program_header->p_filesz;
	printf(_WW_YELLOW_COLOR "\n\nInjection start offset: %lx, stubsizewpad: %ld\n", _injection_addr, _stub_size_with_pad);

	// Getting next segment's header
	Elf64_Phdr *_next_program_header =
		(Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);

	printf("NEW Segment size: %lu bytes\n\n" _WW_RESET_COLOR, (unsigned long)_program_header->p_filesz);

	// printf("%s\n", section_header->sh_name);
	// TODO REPLACE STRCMP
	Elf64_Shdr *strtab = get_section_header(_mapped_data, _elf_header->e_shstrndx);

	Elf64_Shdr *_last_text_shdr = NULL;

	// Get a pointer to the first section header
	Elf64_Shdr *section_header1 = (Elf64_Shdr *)(_mapped_data + _elf_header->e_shoff);
	for (size_t j = 0; section_header1[j].sh_offset < _next_program_header->p_offset;
		 ++j)
	{
		_last_text_shdr = &section_header1[j];
		Elf64_Shdr *shdr = get_section_header(_mapped_data, j);
		char *section_name = (char *)(_mapped_data + strtab->sh_offset + shdr->sh_name);
		printf("=======>shname= %s\n", section_name);
	}
	if (_last_text_shdr)
	{
		printf("====================last txt\n");
		_last_text_shdr->sh_size += sizeof(_stub);
	}

	size_t j = 1;
	while (j < _elf_header->e_phnum)
	{
		Elf64_Phdr	*_curr_program_header =
			(Elf64_Phdr *)((void *)_program_header + j * _elf_header->e_phentsize);
		if ((off_t)_curr_program_header->p_vaddr > _injection_addr)
		{
			printf("=================>PH  size: %ld\n", _stub_size_with_pad);
			_curr_program_header->p_offset += _stub_size_with_pad;
		}
		++j;
	}

	// Get a pointer to the first section header
	Elf64_Shdr *section_header = (Elf64_Shdr *)(_mapped_data + _elf_header->e_shoff);

	/* We iterate over each section header to find .text section name */
	for (size_t j = 0; j < _elf_header->e_shnum; j++)
	{
		Elf64_Shdr *shdr = get_section_header(_mapped_data, j);
		char *section_name = (char *)(_mapped_data + strtab->sh_offset + shdr->sh_name);
		printf("shname= %s\n", section_name);
		// if (strcmp(section_name, ".text") == 0)
		// {
		// 	for (size_t k = ++j; k < _elf_header->e_shnum; k++)
		// 	{
				// strtab->sh_size += sizeof(_stub);
				shdr = get_section_header(_mapped_data, j);
				section_name = (char *)(_mapped_data + strtab->sh_offset + shdr->sh_name);
				printf("IN shname= %s, addr: %lx\n", section_name,shdr->sh_addr);
				if ((off_t)shdr->sh_addr > _injection_addr){
					printf("stub size with pad added!\n");
					shdr->sh_offset += _stub_size_with_pad;	section_name = (char *)(_mapped_data + strtab->sh_offset + shdr->sh_name);
				// }
			// }
			// break;
		}
	}
	_elf_header->e_shoff += _stub_size_with_pad;
}

void	_ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header, size_t i)
{
	// Inject the stub only if the segment is executable and loadable.
	if (_program_header->p_type == PT_LOAD &&
		(_program_header->p_flags & PF_X))
	{
		off_t	_injection_offset =
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

		// if ((off_t)sizeof(_stub) <= _padding_size)
		// {
			// _ww_memcpy(_mapped_data + _injection_offset, _stub, sizeof(_stub));

			Elf64_Ehdr	*_elf_header = (Elf64_Ehdr *)_mapped_data;
			off_t	entry_offset =
				_program_header->p_vaddr + _program_header->p_filesz - _elf_header->e_entry;
			printf("e_entry offset: %lx\n", entry_offset);

			// off_t		_text_section_addr = 0;
			// Elf64_Shdr	*_strtab = get_section_header(_mapped_data, _elf_header->e_shstrndx);
			/* We iterate over each section header to find .text section name */
			// for (size_t i = 0; i < _elf_header->e_shnum; i++) {
			// 	Elf64_Shdr *shdr = get_section_header(_mapped_data, i);
			// 	char *section_name = (char *)(_mapped_data + _strtab->sh_offset + shdr->sh_name);
			// 	printf("shname= %s\n", section_name);
			// 	if (strcmp(section_name, ".text") == 0) _text_section_addr = shdr->sh_addr;
			// // }
			// off_t	_segment_offset = _program_header->p_memsz;

			// printf("============>%lx %lx\n", entry_offset, _segment_offset);
			
			_elf_header->e_entry = _injection_addr;
			printf("e_entry address: %lx\n", _elf_header->e_entry);
			_program_header->p_filesz += sizeof(_stub);
			_program_header->p_memsz += sizeof(_stub);
			printf("NEW Segment size: %lu bytes\n\n" _WW_RESET_COLOR, (unsigned long)_program_header->p_filesz);
		// }
		// else {
		// if ((off_t)sizeof(_stub) > _padding_size) {
			printf(
				"Padding size is smaller than the code to be injected."
				);
			_ww_shift_offsets_after_stub_insertion(_elf_header, _program_header, i);

	size_t _stub_size_with_pad = ((sizeof(_stub) /_WW_PAGE_SIZE) + 1) * _WW_PAGE_SIZE;

				// TODO MALLOC
	unsigned char   _stub_with_pad[_stub_size_with_pad];
	void                    *_file_with_stub = malloc(_file_size + _stub_size_with_pad);//TODO protect+free
	// memset(_file_with_stub + _injection_addr, 0, _file_size + _stub_size_with_pad); //TODO REPLACE BZERO
	memset(_stub_with_pad, 0, _stub_size_with_pad);
	_ww_memcpy(_stub_with_pad, _stub, sizeof(_stub));
	printf("** varstubszwith pad: %ld;  stub SIZE with pad: %ld; file SIZE: %ld, injcaddr: %lx\n", _stub_size_with_pad,sizeof(_stub_with_pad), _file_size, _injection_addr);
	memset(_file_with_stub, 0, _file_size + _stub_size_with_pad);
	_ww_memcpy(_file_with_stub, _mapped_data, _injection_offset);
	_ww_memcpy(
			_file_with_stub + _injection_offset + _stub_size_with_pad,
			_mapped_data + _injection_offset,
			_file_size - _injection_offset
	);
	_ww_memcpy(_file_with_stub + _injection_offset, _stub_with_pad, _stub_size_with_pad);

	// Unmap the file from memory
	if (munmap(_mapped_data, _file_size) < 0)
			return _ww_print_errors(_WW_ERR_MUNMAP);

	_mapped_data = _file_with_stub;
		// }
	}


}