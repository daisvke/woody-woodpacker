#include "ww.h"

const uint8_t payload[] = {
	// 0x55,                         // push   rbp
	// 0x48, 0x89, 0xe5,             // mov    rbp,rsp
	// 0xb8, 0x2a, 0x00, 0x00, 0x00, // mov    eax,0x2a
	// 0x5d,                         // pop    rbp
	// 0xc3,                         // ret
	//=================================//
	// 0xb8, 0x3c, 0x00, 0x00, 0x00, // mov eax, 60
	// 0xbf, 0x2a, 0x00, 0x00, 0x00, // mov edi, 42
	// 0x0f, 0x05,                   // syscall
	//=================================//
	0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, // mov rax, 60
	0x48, 0xc7, 0xc7, 0x2a, 0x00, 0x00, 0x00, // mov rdi, 42
	0x0f, 0x05,                               // syscall
};
    // \x48\xC7\xC0\x3C\x00\x00\x00   ; mov rax, 60
    // \x48\xC7\xC7\x2A\x00\x00\x00   ; mov rdi, status
    // \x0F\x05                       ; syscall
//============================//
//     \xB8\x3C\x00\x00\x00   ; mov eax, 60
//     \xBF\x2A\x00\x00\x00   ; mov edi, 42
//     \x0F\x05               ; syscall

//============================//
//   401107:       48 89 e5                mov    rbp,rsp
//   40110a:       b8 2a 00 00 00          mov    eax,0x2a
//   40110f:       5d                      pop    rbp
//   401110:       c3                      ret

Elf64_Shdr *get_section_header(void *f, int idx) {
	return f + (((Elf64_Ehdr *)f)->e_shoff + (idx * ((Elf64_Ehdr *)f)->e_shentsize ));
}
Elf64_Phdr *get_program_header(void *f, int idx) {
	return f + (((Elf64_Ehdr *)f)->e_phoff + (idx * ((Elf64_Ehdr *)f)->e_phentsize ));
}


Elf64_Shdr *get_section_header_by_name(void *f, char *section_name, int *idx) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)f;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them.
	*/
	Elf64_Shdr *strtab = get_section_header(f, ehdr->e_shstrndx);

	/* We iterate over each section header to find .text section name */
	for (size_t i = 1; i < ehdr->e_shnum; i++) {
		Elf64_Shdr *shdr = get_section_header(f, i);
		char *sh_name = (char *)(f + strtab->sh_offset + shdr->sh_name);
		if (strcmp(sh_name, section_name) == 0) {
			*idx = i;
			return shdr;
		}
	}
	return NULL;
}

void overwrite_text_section(Elf64_Shdr *txt_shdr) {
	unsigned char *to_modify = _buffer + txt_shdr->sh_offset;
	for (size_t i = 0; i < sizeof(payload); i++) {
		to_modify[i] = payload[i];
	}
}

// // Allouer une zone de mémoire exécutable
// void *executableMemory = mmap(NULL, length, PROT_READ | PROT_WRITE | PROT_EXEC,
// 							  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
// if (executableMemory == MAP_FAILED)
// {
// 	perror("mmap");
// 	return 1;
// }
// memcpy(executableMemory, payload, length);
// // Appeler la fonction payload
// int (*shell)() = (int (*)())executableMemory;
// int ret = shell();

int create_file(void* towrite, size_t newbinsz)
{
    // 0755: rwx for owner, rx for group and others
    int _outfile_fd = open("newbin", O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (_outfile_fd == -1) {
        // Unmap the file from memory
        return _ww_print_errors(_WW_ERR_OUTFILE);
    }

    ssize_t _bytes_written = write(_outfile_fd, towrite, newbinsz);
    if (_bytes_written < 0) _ww_print_errors(_WW_ERR_WRITEFILE);
    close(_outfile_fd);

    return 0;
}

void forge_newbin(void *newbin, size_t newbinsz, void *f, size_t fsz, size_t sectionsz) {
	printf("sectionsz: %ld\n", sectionsz);

	// Set all the bytes to 0
	memset(newbin, 0, newbinsz);

	// Get .bss section header
	int shdr_idx = 0;
	Elf64_Shdr *bss_shdr = get_section_header_by_name(f, ".bss", &shdr_idx);
	printf(".bss size = %lx\n", bss_shdr->sh_size);
	printf("shdr_idx = %d\n", shdr_idx);

	// Copy f bytes until .bss section end
	Elf64_Ehdr *fehdr = (Elf64_Ehdr *)f;
	printf("offset where our section start : %lx\n", bss_shdr->sh_offset);
	printf("payload size in hex : %lx\n", sizeof(payload));
	size_t nb_bytes_to_copy = bss_shdr->sh_offset + bss_shdr->sh_size;
	memcpy(newbin, f, nb_bytes_to_copy);

	// Copy bytecodes
	unsigned char *tmp = newbin + bss_shdr->sh_offset + bss_shdr->sh_size;
	for (size_t i = 0; i < sizeof(payload); i++)
		tmp[i] = payload[i];

	// Copy the rest of the file
	size_t save = nb_bytes_to_copy;
	nb_bytes_to_copy = fsz - (bss_shdr->sh_offset + bss_shdr->sh_size);
	printf("save + nb_bytes_to_copy = %ld\n", save + nb_bytes_to_copy);
	printf("fsz = %ld\n", fsz);
	off_t nboffset = bss_shdr->sh_offset + bss_shdr->sh_size + sizeof(payload);
	off_t foffset = bss_shdr->sh_offset + bss_shdr->sh_size;
	memcpy(newbin + nboffset, f + foffset, nb_bytes_to_copy);

	// Edit new binary Elf header (shoff, shnum, ...)
	Elf64_Ehdr *nbehdr = (Elf64_Ehdr *)newbin;
	nbehdr->e_shoff = fehdr->e_shoff + sectionsz;
	nbehdr->e_shnum += 1;
	// Change entry point
	// nbehdr->e_entry = bss_shdr->sh_addr + bss_shdr->sh_size;
	nbehdr->e_entry = 0x4040b0;
	// nbehdr->e_shstrndx = ;

	// Shift section offset in the sections headers
	for (size_t i = shdr_idx; i < fehdr->e_shnum; i++) {
		Elf64_Shdr *shdr = get_section_header(newbin, i);
		shdr->sh_offset += sectionsz;
	}

	// Create the section header for the new section
	Elf64_Shdr *nbshdr = get_section_header(newbin, nbehdr->e_shnum - 1);
	nbshdr->sh_type = SHT_PROGBITS;
	nbshdr->sh_offset = bss_shdr->sh_offset + bss_shdr->sh_size;
	nbshdr->sh_size = sectionsz;
	nbshdr->sh_flags |= SHF_EXECINSTR;
	nbshdr->sh_flags |= SHF_ALLOC;
	nbshdr->sh_addralign = 16;
	nbshdr->sh_addr = bss_shdr->sh_addr + bss_shdr->sh_size;

	// Update the program header
	// filesz += bss size + sizeof(payload)
	// memsz += sizeof(payload)

	int phdrnum = 5; // TODO: create the function that return the correct phdrnum needed
	Elf64_Phdr *nbphdr = get_program_header(newbin, phdrnum);
	(void)nbphdr;
	nbphdr->p_filesz += bss_shdr->sh_size + sectionsz;
	nbphdr->p_memsz += sectionsz;
	nbphdr->p_flags |= PF_X;
}

void	_ww_process_mapped_data()
{
	size_t sz = _file_size + sizeof(payload) + sizeof(Elf64_Shdr);
	unsigned char *newbin = malloc(sz);
	if (!newbin) {
		perror("malloc");
		return;
	}
	forge_newbin(newbin, sz, _buffer, _file_size, sizeof(payload));
	create_file(newbin, sz);
	free(newbin);
}
