#include "ww.h"

void _ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header, size_t i)
{
	// Inject the stub only if the segment is executable and loadable.
	if (_program_header->p_type == PT_LOAD &&
		(_program_header->p_flags & PF_X))
	{
   		size_t _stub_size_with_pad = ((sizeof(STUB)/16) +1)*16;

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
		printf("code size: %ld\n", sizeof(STUB));

		if ((off_t)_stub_size_with_pad <= _padding_size)
		{
			// TODO MALLOC
			unsigned char	_stub_with_pad[_stub_size_with_pad];
			// printf("stub SIZE with pad: %ld; file SIZE: %ld\n", _stub_size_with_pad);
			bzero(_stub_with_pad, 0); //TODO REPLACE BZERO
			_ww_memcpy(
				_mapped_data + _injection_addr,
				_mapped_data + _injection_addr + _stub_size_with_pad,
				_file_size - _injection_addr - _stub_size_with_pad
			);
			_ww_memcpy(_stub_with_pad, _mapped_data, _stub_size_with_pad);

			Elf64_Ehdr *_elf_header = (Elf64_Ehdr *)_mapped_data;
			_elf_header->e_entry = _program_header->p_vaddr + _program_header->p_filesz;
			printf("e_entry address: %lx\n", _elf_header->e_entry);
			_program_header->p_filesz += sizeof(STUB);
			_program_header->p_memsz += sizeof(STUB);
			printf("NEW Segment size: %lu bytes\n\n" _WW_RESET_COLOR, (unsigned long)_program_header->p_filesz);

			while (++i < _elf_header->e_phnum) {
				_program_header->p_offset += _stub_size_with_pad;
				_program_header =
					(Elf64_Phdr *)((void *)_program_header + _elf_header->e_phentsize);
			}
			for (size_t i = 0; i < _elf_header->e_shnum; i++)
			{
				// printf("%s\n", section_header->sh_name);
				// TODO REPLACE STRCMP
				Elf64_Ehdr *ehdr = (Elf64_Ehdr *)_mapped_data;
				Elf64_Shdr *strtab = get_section_header(_mapped_data, ehdr->e_shstrndx);
  
				/* We iterate over each section header to find .text section name */
				for (size_t i = 1; i < ehdr->e_shnum; i++) {
					Elf64_Shdr *shdr = get_section_header(_mapped_data, i);
					char *sh_name = (char *)(_mapped_data + strtab->sh_offset + shdr->sh_name);
					if (strcmp(sh_name, ".text") == 0)
					{
						for (size_t j = ++i; j < ehdr->e_shnum; j++)
							// strtab->sh_size += sizeof(STUB);
							strtab->sh_offset += _stub_size_with_pad;
						break;
					}
				}
			}
			_elf_header->e_shoff += _stub_size_with_pad;
		}
		else
		{
			printf(
				"Padding size is smaller than the code to be injected."
				"Aborting...");
		}
	}
}