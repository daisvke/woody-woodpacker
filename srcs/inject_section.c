#include "ww.h"

const uint8_t	payload[] = {
	// 0x55,                                  // push   rbp
	// 0x48, 0x89, 0xe5,                      // mov    rbp,rsp
	// 0xb8, 0x2a, 0x00, 0x00, 0x00,          // mov    eax,0x2a
	// 0x5d,                                  // pop    rbp
	// 0xc3,                                  // ret
	//=================================//
	// 0xb8, 0x3c, 0x00, 0x00, 0x00,          // mov eax, 60
	// 0xbf, 0x2a, 0x00, 0x00, 0x00,          // mov edi, 42
	// 0x0f, 0x05,                            // syscall
	//=================================//
	0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, // mov rax, 60
	0x48, 0xc7, 0xc7, 0x2a, 0x00, 0x00, 0x00, // mov rdi, 42
	0x0f, 0x05,                               // syscall
};

static void	overwrite_text_section(Elf64_Shdr *txt_shdr) {
	unsigned char *to_modify = _buffer + txt_shdr->sh_offset;
	for (size_t i = 0; i < sizeof(payload); i++) {
		to_modify[i] = payload[i];
	}
}

static void	update_elf_header(Elf64_Ehdr *ehdr, Elf64_Off shoff, Elf64_Half shnum, Elf64_Addr entry) {
	ehdr->e_shoff = shoff;
	ehdr->e_shnum = shnum;
	ehdr->e_entry = entry;
}

static void	shift_section_headers_offset(Elf64_Ehdr *ehdr, size_t sectionsz, int shdr_idx) {
	Elf64_Shdr *shdr = get_section_header(ehdr, shdr_idx);
	for (size_t i = shdr_idx; i < ehdr->e_shnum; i++) {
		Elf64_Shdr *shdr = get_section_header(ehdr, i);
		shdr->sh_offset += sectionsz;
	}
}

static void	create_woody_section_header(void *woody, Elf64_Off offset, Elf64_Xword size, Elf64_Addr addr) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)woody;
	Elf64_Shdr *shdr = get_section_header(ehdr, ehdr->e_shnum - 1);
	shdr->sh_type = SHT_PROGBITS;
	shdr->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	shdr->sh_addr = addr;
	shdr->sh_offset = offset;
	shdr->sh_size = size;
	shdr->sh_addralign = 16;
}

static void	update_woody_program_header(void *woody, int phdrnum, size_t sectionsz, size_t bsssz) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)woody;
	Elf64_Phdr *phdr = get_program_header(ehdr, phdrnum);
	phdr->p_flags |= PF_X ;
	phdr->p_filesz += bsssz + sectionsz;
	phdr->p_memsz += sectionsz;
}

void	forge_woody(void *woody, size_t woodysz, void *f, size_t fsz, size_t sectionsz) {

	// Set all the bytes of the woody to 0
	memset(woody, 0, woodysz);

	// Get .bss section header
	int shdr_idx = 0;
	Elf64_Shdr *bss_shdr = get_section_header_by_name(f, ".bss", &shdr_idx);
	// TODO: check if bss_shdr has been found

	// Copy f bytes until .bss section end
	Elf64_Ehdr *fehdr = (Elf64_Ehdr *)f;
	// Our section will start at the end of the .bss section
	memcpy(woody, f, bss_shdr->sh_offset + bss_shdr->sh_size);

	// Copy the payload
	unsigned char *tmp = woody + bss_shdr->sh_offset + bss_shdr->sh_size;
	memcpy(tmp, payload, sizeof(payload));

	// Copy the rest of the file
	size_t nb_bytes_to_copy = fsz - (bss_shdr->sh_offset + bss_shdr->sh_size);
	off_t nboffset = bss_shdr->sh_offset + bss_shdr->sh_size + sizeof(payload);
	off_t foffset = bss_shdr->sh_offset + bss_shdr->sh_size;
	memcpy(woody + nboffset, f + foffset, nb_bytes_to_copy);

	// Update woody elf header (section headers offset, number of sections and entry point)
	update_elf_header(woody, fehdr->e_shoff + sectionsz, fehdr->e_shnum + 1, bss_shdr->sh_addr + bss_shdr->sh_size);

	// Shift section offset in the sections headers
	// Because we added a section before some other sections
	shift_section_headers_offset(woody, sectionsz, shdr_idx);

	// Create the section header for the new section that contains our payload
	create_woody_section_header(woody, bss_shdr->sh_offset + bss_shdr->sh_size, sectionsz, bss_shdr->sh_addr + bss_shdr->sh_size);

	int phdrnum = 5; // TODO: create the function that return the correct phdrnum needed
	update_woody_program_header(woody, phdrnum, sectionsz, bss_shdr->sh_size);
	Elf64_Phdr *nbphdr = get_program_header(woody, phdrnum);
}

void	*inject_section(size_t *woodysz) {
	/* Compute what will be the size of `woody` and allocate mem */
	*woodysz = _file_size + sizeof(payload) + sizeof(Elf64_Shdr);
	void *woody = malloc(*woodysz);
	if (!woody) {
		_ww_print_errors(_WW_ERR_ALLOCMEM);
		return NULL;
	}

	// memset

	// Get the section, our section will be injected after this one

	// Copy the file until the section

	// Copy the payload

	// Copy the rest of the file

	// Update the Elf header

	// Shift section offset in the sections headers

	// Create the section header for the new section

	// Update the program header

	// Write the new file

	forge_woody(woody, *woodysz, _buffer, _file_size, sizeof(payload));
	// create_the_woody(woody, sz);
	// free(woody);
	return woody;
}
