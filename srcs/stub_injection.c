#include "ww.h"
#include "stub.h"

void _ww_shift_offsets_after_stub_insertion(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header, size_t i, size_t _stub_size_with_pad)
{

	off_t _injection_addr = _program_header->p_vaddr + _program_header->p_filesz;
	printf(_WW_YELLOW_COLOR "\n\nInjection start offset: %lx\n", _injection_addr);

	// Getting next segment's header
	Elf64_Phdr *_next_program_header =
		(Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);

	printf("e_entry address: %lx\n", _elf_header->e_entry);
	_elf_header->e_entry = _program_header->p_vaddr + _program_header->p_filesz;
	printf("NEW e_entry address: %lx\n", _elf_header->e_entry);
	_program_header->p_filesz += sizeof(_stub);
	_program_header->p_memsz += sizeof(_stub);
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
		printf("==============================qdsfqsdfsdfsdfqsf\n");
		_last_text_shdr->sh_size += sizeof(_stub);
	}

	size_t j = 1;
	Elf64_Phdr *_curr_program_header = (Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);
	while (j < _elf_header->e_phnum)
	{
		_curr_program_header =
			(Elf64_Phdr *)((void *)_program_header + j * _elf_header->e_phentsize);
		if ((off_t)_curr_program_header->p_offset > _injection_addr)
			_curr_program_header->p_offset += _stub_size_with_pad;
		++j;
	}

	// Get a pointer to the first section header
	Elf64_Shdr *section_header = (Elf64_Shdr *)(_mapped_data + _elf_header->e_shoff);

	/* We iterate over each section header to find .text section name */
	for (size_t i = 0; i < _elf_header->e_shnum; i++)
	{
		Elf64_Shdr *shdr = get_section_header(_mapped_data, i);
		char *section_name = (char *)(_mapped_data + strtab->sh_offset + shdr->sh_name);
		printf("shname= %s\n", section_name);
		if (strcmp(section_name, ".text") == 0)
		{
			for (size_t j = ++i; j < _elf_header->e_shnum; j++)
			{
				// strtab->sh_size += sizeof(_stub);
				shdr = get_section_header(_mapped_data, j);
				shdr->sh_offset += _stub_size_with_pad;
			}
			break;
		}
	}
	_elf_header->e_shoff += _stub_size_with_pad;
}

void _ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header, size_t i)
{
	// Inject the stub only if the segment is executable and loadable.
	if (_program_header->p_type == PT_LOAD &&
		(_program_header->p_flags & PF_X))
	{
		size_t _stub_size_with_pad = ((sizeof(_stub) / 16) + 1) * 16;

		// off_t _injection_offset =
		// 	_program_header->p_offset + _program_header->p_filesz;
		off_t _injection_addr = _program_header->p_vaddr + _program_header->p_filesz;
		printf(_WW_YELLOW_COLOR "\n\nInjection start offset: %lx\n", _injection_addr);

		// Getting next segment's header
		Elf64_Phdr *_next_program_header =
			(Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);
		off_t _padding_size = _next_program_header->p_vaddr - _injection_addr;

		printf("Next segment offset: %lx\n", _next_program_header->p_offset);
		printf("Padding size: %ld\n", _padding_size);
		printf("code size: %ld\n", sizeof(_stub));

		if ((off_t)_stub_size_with_pad <= _padding_size)
		{
			// TODO MALLOC
			unsigned char _stub_with_pad[_stub_size_with_pad];
			void *_file_with_stub = malloc(_file_size + _stub_size_with_pad); // TODO protect+free
			printf("stub SIZE with pad: %ld; file SIZE: %ld\n", _stub_size_with_pad, _file_size);
			bzero(_stub_with_pad, 0); // TODO REPLACE BZERO
			_ww_shift_offsets_after_stub_insertion(_elf_header, _program_header, i, _stub_size_with_pad);

			_ww_memcpy(_file_with_stub, _mapped_data, _injection_addr);
			_ww_memcpy(
				(char *)_file_with_stub + _injection_addr + _stub_size_with_pad,
				(char *)_mapped_data + _injection_addr,
				_file_size - _injection_addr);
			_ww_memcpy(_file_with_stub + _injection_addr, _stub_with_pad, _stub_size_with_pad);

			// Unmap the file from memory
			if (munmap(_mapped_data, _file_size) < 0)
				return _ww_print_errors(_WW_ERR_MUNMAP);

			_mapped_data = _file_with_stub;
		}
		else
		{
			printf(
				"Padding size is smaller than the code to be injected."
				"Aborting...");
		}
	}
}