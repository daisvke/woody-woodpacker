#include "ww.h"
#include "stub.h"

// This function is responsible for adjusting various offsets and sizes
// in the program headers, and section headers after inserting the stub code.
void _ww_shift_offsets_for_stub_insertion(
	Elf64_Ehdr *_elf_header, off_t _injection_offset)
{
	size_t _stub_size_with_pad = ((sizeof(_stub) / _WW_PAGE_SIZE) + 1) * _WW_PAGE_SIZE;
	Elf64_Phdr *_program_header = (Elf64_Phdr *)(_mapped_data + _elf_header->e_phoff);
	// Increase phdr's offset by PAGE_SIZEF for each phdr after the insertion
	for (size_t j = 0; j < _elf_header->e_phnum; j++)
		if ((off_t)_program_header[j].p_offset > _injection_offset)
			_program_header[j].p_offset += _stub_size_with_pad;

	Elf64_Shdr *_section_header = (Elf64_Shdr *)(_mapped_data + _elf_header->e_shoff);
	Elf64_Shdr *_last_text_shdr = NULL;
	// Increase size of the last shdr in the .text segment by the stub length
	for (size_t j = 0; _section_header[j].sh_offset < _program_header[1].p_offset; ++j)
		_last_text_shdr = &_section_header[j];
	if (_last_text_shdr)
		_last_text_shdr->sh_size += sizeof(_stub);
	// Increase shdr's offset by PAGE_SIZEF for each shdr after the insertion
	for (size_t j = 0; j < _elf_header->e_shnum; j++)
		if ((off_t)_section_header[j].sh_offset > _injection_offset)
			_section_header[j].sh_offset += _stub_size_with_pad;
	// TODO only if section header exists
	_elf_header->e_shoff += _stub_size_with_pad;
}

void _ww_generate_new_file_with_parasite(off_t _injection_offset)
{
	size_t _stub_size_with_pad = ((sizeof(_stub) / _WW_PAGE_SIZE) + 1) * _WW_PAGE_SIZE;

	// Add padding to the stub by the page size of the memory
	unsigned char *_stub_with_pad = malloc(_stub_size_with_pad);
	if (!_stub_with_pad)
		_ww_print_error_and_exit(_WW_ERR_ALLOCMEM);
	_ww_memset(_stub_with_pad, 0, _stub_size_with_pad);
	_ww_memcpy(_stub_with_pad, _stub, sizeof(_stub));

	// Create a new file with the mapped data + stub with padding
	void *_file_with_stub = malloc(_file_size + _stub_size_with_pad);
	if (!_file_with_stub)
		_ww_print_error_and_exit(_WW_ERR_ALLOCMEM);
	// Copy from the mapped data until the injection point of the stub
	_ww_memcpy(_file_with_stub, _mapped_data, _injection_offset);
	// Copy from the end of the stub(with padding) to the end of the mapped data
	_ww_memcpy(
		_file_with_stub + _injection_offset + _stub_size_with_pad,
		_mapped_data + _injection_offset,
		_file_size - _injection_offset);
	// Copy the stub with the padding
	_ww_memcpy(_file_with_stub + _injection_offset, _stub_with_pad, _stub_size_with_pad);

	free(_stub_with_pad);
	// Unmap the file from memory
	if (munmap(_mapped_data, _file_size) < 0)
		_ww_print_error_and_exit(_WW_ERR_MUNMAP);
	// Link the new file to the global variable
	_mapped_data = _file_with_stub;
}

void _ww_inject_stub(Elf64_Ehdr *_elf_header, Elf64_Phdr *_program_header)
{
	// Inject the stub only if the segment is executable and loadable.
	if (_program_header->p_type == PT_LOAD && (_program_header->p_flags & PF_X))
	{
		off_t _injection_offset = _program_header->p_offset + _program_header->p_filesz;
		off_t _injection_addr = _program_header->p_vaddr + _program_header->p_filesz;
		off_t _padding_size = _program_header[1].p_offset - _injection_offset;
		off_t _entry_offset =
			_program_header->p_vaddr + _program_header->p_filesz - _elf_header->e_entry;

		// Update the entry point of the file to the stub's injection address
		_elf_header->e_entry = _injection_addr;
		// Update the current phdr size that contains the stub
		_program_header->p_filesz += sizeof(_stub);
		_program_header->p_memsz += sizeof(_stub);

		printf(_WW_YELLOW_COLOR "Injection start offset: %lx\n", _injection_offset);
		printf("Padding size: %ld\n", _padding_size);
		printf("Stub size: %ld\n", sizeof(_stub));
		printf("e_entry offset from stub: %lx\n", _entry_offset);
		printf("e_entry address: %lx\n\n", _elf_header->e_entry);

		// Insert inside executable segment's end padding if there is sufficent space
		if ((off_t)sizeof(_stub) <= _padding_size)
		{
			printf("The shellcode is injected into the executable segment's padding.\n" _WW_RESET_COLOR);
			_modes |= _WW_INJECTREG_PADDING;
			_ww_memcpy(_mapped_data + _injection_offset, _stub, sizeof(_stub));
		}
		else
		{
			printf("The executable segment's padding size is smaller than the code"
				   "to be injected.\nThe shellcode will be injected anyway and all data"
				   "following the injection point will be shifted.\n" _WW_RESET_COLOR);
			_ww_shift_offsets_for_stub_insertion(_elf_header, _injection_offset);
			_ww_generate_new_file_with_parasite(_injection_offset);
		}
	}
}