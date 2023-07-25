#include "ww.h"

void	_ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header)
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

		if ((off_t)sizeof(_stub) <= _padding_size)
		{
			_ww_memcpy(_mapped_data + _injection_offset, _stub, sizeof(_stub));

			Elf64_Ehdr	*_elf_header = (Elf64_Ehdr *)_mapped_data;
			_elf_header->e_entry = _program_header->p_vaddr + _program_header->p_filesz;
			printf("e_entry address: %lx\n" _WW_RESET_COLOR, _elf_header->e_entry);
			// _program_header->p_filesz += sizeof(_stub);
			_program_header->p_memsz += sizeof(_stub);
			// printf("NEW Segment size: %lu bytes\n\n" _WW_RESET_COLOR, (unsigned long)_program_header->p_filesz);
		}
		else {
			printf(
				"Padding size is smaller than the code to be injected."
				"Aborting..."
				);
		}
	}
}