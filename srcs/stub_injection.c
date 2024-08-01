#include "ww.h"
#include "stub.h"

// This function is responsible for adjusting various offsets and sizes
// in the program headers, and section headers after inserting the stub code.
void ww_shift_offsets_for_stub_insertion(
	Elf64_Ehdr *elf_header, Elf64_Off injection_offset)
{
	size_t stub_size_with_pad = ((sizeof(g_stub) / WW_PAGE_SIZE) + 1) * WW_PAGE_SIZE;

	// First, shift the program headers
	Elf64_Phdr	*program_header = (Elf64_Phdr *)(g_mapped_data + elf_header->e_phoff);
	// Increase phdr's offset by PAGE_SIZEF for each phdr after the insertion
	for (size_t i = 0; i < elf_header->e_phnum; i++)
		if (program_header[i].p_offset > injection_offset)
			program_header[i].p_offset += stub_size_with_pad;

	// Then, shift the section headers
	// No need to shift from the section header if there is no section header
	if (elf_header->e_shnum == 0)
		return;
	Elf64_Shdr	*section_header = (Elf64_Shdr *)(g_mapped_data + elf_header->e_shoff);
	// Increase shdr's offset by PAGE_SIZE for each shdr after the insertion
	for (size_t i = 0; i < elf_header->e_shnum; i++)
		if (section_header[i].sh_offset > injection_offset)
			section_header[i].sh_offset += stub_size_with_pad;
	if (elf_header->e_shoff > injection_offset)
		elf_header->e_shoff += stub_size_with_pad;
}

void ww_generate_new_file_with_parasite(Elf64_Off injection_offset)
{
	size_t	stub_size_with_pad = ((sizeof(g_stub) / WW_PAGE_SIZE) + 1) * WW_PAGE_SIZE;
	// Create a new file that can receive the mapped data + stub with padding
	void *file_with_stub = malloc(g_file_size + stub_size_with_pad);
	if (!file_with_stub) ww_print_error_and_exit(WW_ERR_ALLOCMEM);
	// Copy from the mapped data until the injection point of the stub
	ww_memcpy(file_with_stub, g_mapped_data, injection_offset);
	ww_memset(file_with_stub + injection_offset, 0, stub_size_with_pad);
	// Copy from the end of the stub(with padding) to the end of the mapped data
	ww_memcpy(
		file_with_stub + injection_offset + stub_size_with_pad,
		g_mapped_data + injection_offset,
		g_file_size - injection_offset);
	// Copy the stub with the padding
	ww_memcpy(file_with_stub + injection_offset, g_stub, sizeof(g_stub));
	// Unmap the file from memory
	if (munmap(g_mapped_data, g_file_size) < 0) ww_print_error_and_exit(WW_ERR_MUNMAP);
	// Link the new file to the global variable
	g_mapped_data = file_with_stub;
	g_file_size += stub_size_with_pad;
}

void ww_patch_stub(char *key, const ww_t_patch *patch, Elf64_Off injection_offset)
{
	// Get the offset of the patch injection location in the stub
	Elf64_Off	patch_offset =
		injection_offset + (sizeof(g_stub) - (sizeof(ww_t_patch) + WW_KEYSTRENGTH + 1));
	// Patch the stub with the actual computed data
	ww_memcpy(g_mapped_data + patch_offset, patch, sizeof(ww_t_patch));
	// Get the offset of the patch injection location in the stub
	Elf64_Off	patch_key_offset =
		injection_offset + sizeof(g_stub) - WW_KEYSTRENGTH - 1;
	// Patch the stub with the actual computed data
	ww_memcpy(g_mapped_data + patch_key_offset, key, WW_KEYSTRENGTH);
}

void ww_padding_injection(Elf64_Off injection_offset)
{
	if (g_modes & WW_VERBOSE)
		printf(WW_GREEN_COLOR "The shellcode is injected into the executable "
			   "segment's padding.\n" WW_RESET_COLOR);
	g_modes |= WW_INJECTREG_PADDING;
	ww_memcpy(g_mapped_data + injection_offset, g_stub, sizeof(g_stub));
}

void ww_shifting_injection(Elf64_Ehdr *elf_header, Elf64_Off injection_offset)
{
	if (g_modes & WW_VERBOSE)
		printf(WW_GREEN_COLOR "The executable segment's padding size is smaller than the code "
			   "to be injected.\nThe shellcode will be injected anyway and all data"
			   " following the injection point will be shifted.\n" WW_RESET_COLOR);
	ww_shift_offsets_for_stub_insertion(elf_header, injection_offset);
	ww_generate_new_file_with_parasite(injection_offset);
}

void ww_inject_stub(Elf64_Ehdr *elf_header, Elf64_Phdr *program_header, char *key)
{
	/* The injection offset is the sum of the executable LOAD segment's offset
	 *  and the file size of that segment, which equals to the end of that segment.
	 *
	 *               Segment (ph[0])   { stub }
	 * |------------------------------X----------|
	 * p_offset              p_offset + p_filesz
	 *                      <=> stub_injection_off  
	 *
	 */
	Elf64_Off	injection_offset = program_header->p_offset + program_header->p_filesz;
	// To compute the injection address we replace the offset by the segment's adress
	Elf64_Addr	injection_addr = program_header->p_vaddr + program_header->p_filesz;
	/* The padding of the LOAD executable segment is the difference between
	 *  the offset of the n + 1 segment in the file - injection offset (in n segment)
	 *
	 *               n Segment (ph[0])                   n + 1 Segment (ph[1])
	 * |--------------------------X--------------|-----------------------------------|
	 *                      injection_off  ph[1].p_offset
	 */
	int			padding_size = program_header[1].p_offset - injection_offset;
	// Padding size cannot be negative
	padding_size = padding_size > 0 ? padding_size : 0;
	/* Compute the offset of the main function from the stub by substracting
	 *  the address of the entry point from the file size od the segment.
	 * On the diagram below, you can see that the result is the distance
	 *  between e_entry_off and p_vaddr + p_filesz (*)
	 *
	 *                           LOAD executable Segment:
	 *
	 *                                             .text section         
	 *   |--------------------------------------{*****************}*********X-------|
	 * p_vaddr                             e_entry_off                 p_vaddr + p_filesz
	 */
	Elf64_Off	entry_offset =
		injection_addr - elf_header->e_entry;
	Elf64_Shdr	*shdr = ww_get_text_section_header();
	/* The segment offset is the difference between the stub injection offset and
	 *  the LOAD executable segment offset:
	 *
	 *                   Segment        { stub }
	 * |******************************X----------|
	 * p_offset                 injection_off  
	 *
	 */
	Elf64_Off	segment_offset = injection_offset - program_header->p_offset;
	// .text section offset from the injection point
	Elf64_Off	text_offset = injection_offset - shdr->sh_offset;
	// .text section size
	Elf64_Off	text_length = shdr->sh_size;

	// Detect a corrupted program header
	if (elf_header->e_entry > program_header->p_vaddr + program_header->p_memsz) {
		free(key);
		ww_print_error_and_exit(WW_ERR_CORRUPTPHDR);
	}
	// Init patch for the stub's data section. These will be put at the end
	//  of the stub section
	ww_t_patch	patch;
	patch.main_entry_offset_from_stub = entry_offset;
	patch.text_segment_offset_from_stub = segment_offset;
	patch.text_section_offset_from_stub = text_offset;
	patch.text_length = text_length;

	if (g_modes & WW_VERBOSE) // If we have the verbose mode activated
	{
		printf("All values are expressed in hexadecimals\n\n");
		printf("filesize:\t\t%lx\n", g_file_size);
		printf("padding size:\t\t%x\n", padding_size);
		printf("stub size:\t\t%lx\n\n", sizeof(g_stub));

		printf("program_header\n");
		printf("\tp_vaddr:\t%lx\n", program_header->p_vaddr);
		printf("\tp_filesz:\t%lx\n", program_header->p_filesz);
		printf("\tp_offset:\t%lx\n\n", program_header->p_offset);

		printf("section header sh_offset:\t%lx\n", shdr->sh_offset);
		printf("injection start offset:\t\t%lx\n\n", injection_offset);

		printf("main e_entry address:\t\t%lx\n", elf_header->e_entry);
		printf("main e_entry offset from stub:\t%lx\n", entry_offset);
		printf("stub e_entry address:\t\t%lx\n\n", injection_addr);

		printf(".text section offset from stub:\t%lx\n", text_offset);
		printf(".text segment offset from stub:\t%lx\n", segment_offset);
		printf(".text section size:\t\t%lx\n\n", text_length);
	}

	// Update the entry point of the file to the stub's injection address.
	// This is to make the binary start its execution with the stub
	elf_header->e_entry = injection_addr;

	// // Insert inside executable segment's end padding if there is sufficient space
	if ((Elf64_Off)sizeof(g_stub) <= (Elf64_Off)padding_size)
		ww_padding_injection(injection_offset);
	else {// Inject at the end of the .text segment, then shift all the data coming after
		// Update the current phdr size that contains the stub
		program_header->p_filesz += sizeof(g_stub);
		program_header->p_memsz += sizeof(g_stub);
		ww_shifting_injection(elf_header, injection_offset);
	}
	// Insert patch inside the stub
	ww_patch_stub(key, &patch, injection_offset);
	free(key);
}
