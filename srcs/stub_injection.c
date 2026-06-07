#include "ww.h"
#include "stub.h"

// This function is responsible for adjusting various offsets and sizes
// in the program headers, and section headers after inserting the stub code.
void ww_shift_offsets_for_stub_insertion(Elf64_Off injection_offset, size_t sizeof_stub)
{
	size_t		stub_size_with_pad = (sizeof_stub / WW_PAGE_SIZE + 1) * WW_PAGE_SIZE;

	Elf64_Ehdr	*elf_header = (Elf64_Ehdr *)g_mapped_data;
	Elf64_Phdr	*program_header = (Elf64_Phdr *)(g_mapped_data + elf_header->e_phoff);

	for (size_t i = 0; i < elf_header->e_phnum; i++)
	{
		Elf64_Off seg_start = program_header[i].p_offset;
		Elf64_Off seg_end   = program_header[i].p_offset + program_header[i].p_filesz;

		// Shit segments after injection
		if (seg_start > injection_offset)
		{
			program_header[i].p_offset += stub_size_with_pad;
		}

		// Extend segment that contains injection
		if (injection_offset >= seg_start && injection_offset < seg_end)
		{
			program_header[i].p_filesz += stub_size_with_pad;
			program_header[i].p_memsz  += stub_size_with_pad;
		}
	}

	// section headers
	if (elf_header->e_shnum)
	{
		Elf64_Shdr	*section_header =
			(Elf64_Shdr *)(g_mapped_data + elf_header->e_shoff);

		for (size_t i = 0; i < elf_header->e_shnum; i++)
		{
			if (section_header[i].sh_offset > injection_offset)
				section_header[i].sh_offset += stub_size_with_pad;
		}

		if (elf_header->e_shoff > injection_offset)
			elf_header->e_shoff += stub_size_with_pad;
	}

	// Entry adjustment
	if (elf_header->e_entry > injection_offset)
		elf_header->e_entry += stub_size_with_pad;
}

// This is only used along the shifting injection option
void ww_generate_new_file_with_parasite(Elf64_Off injection_offset, size_t sizeof_stub)
{
	size_t	stub_size_with_pad = (sizeof_stub / WW_PAGE_SIZE + 1) * WW_PAGE_SIZE;

	void *file_with_stub = malloc(g_file_size + stub_size_with_pad);
	if (!file_with_stub)
		ww_print_error_and_exit(WW_ERR_ALLOCMEM);

	// Copy before injection point
	ww_memcpy(file_with_stub, g_mapped_data, injection_offset);

	// Insert stub space
	ww_memset(file_with_stub + injection_offset, 0, stub_size_with_pad);

	// Copy after injection point
	ww_memcpy(
		file_with_stub + injection_offset + stub_size_with_pad,
		g_mapped_data + injection_offset,
		g_file_size - injection_offset
	);

	// Copy stub itself
	ww_memcpy(file_with_stub + injection_offset, g_stub, sizeof_stub);

	// Unmap old file
	if (munmap(g_mapped_data, g_file_size) < 0)
		ww_print_error_and_exit(WW_ERR_MUNMAP);

	// Replace global state
	g_mapped_data = file_with_stub;
	g_file_size += stub_size_with_pad;
}

// This is one of the injection options. It does it by injecting the stub at
// the end of the executable segment; then it shifts all the next elements accordingly
void ww_shifting_injection(Elf64_Off injection_offset, size_t sizeof_stub)
{
	printf(
		"Either you have selected the shifting injection mode,"
		" or the executable segment's padding size is smaller than the code "
		"to be injected.\n\nThe shellcode will be injected anyway and all data"
		" following the injection point will be shifted.\n"
	);

	ww_shift_offsets_for_stub_insertion(injection_offset, sizeof_stub);
	ww_generate_new_file_with_parasite(injection_offset, sizeof_stub);
}

// The last lines in the stub file are reserved for necessary data to be patched
void ww_patch_stub(char *key, const ww_t_patch *patch, Elf64_Off injection_offset, size_t sizeof_stub)
{
	Elf64_Off patch_offset =
		injection_offset + (sizeof_stub - (sizeof(ww_t_patch) + WW_KEYSTRENGTH + 1));

	ww_memcpy(g_mapped_data + patch_offset, patch, sizeof(ww_t_patch));

	Elf64_Off patch_key_offset =
		injection_offset + sizeof_stub - WW_KEYSTRENGTH - 1;

	ww_memcpy(g_mapped_data + patch_key_offset, key, WW_KEYSTRENGTH);
}

// This is the default injection option. It injects the shellcode into the
// executable segment's padding. Thus, this option doesn't need any shifting afterward.
void ww_padding_injection(Elf64_Off injection_offset, size_t sizeof_stub)
{
	if (g_modes & WW_VERBOSE)
		printf(
			"   The shellcode will be injected into the executable segment's padding.\n"
		);

	ww_memcpy(
		g_mapped_data + injection_offset,
		g_stub,
		sizeof_stub
	);
}

void ww_compute_injection_values(
	Elf64_Ehdr *elf_header,
	Elf64_Phdr *program_header,
	Elf64_Phdr *next_segment,
	ww_t_injection_value *injection_values,
	ww_t_patch *patch
)
{
	size_t sizeof_stub = sizeof(g_stub);

	injection_values->injection_addr =
		program_header->p_vaddr +
		(injection_values->injection_offset - program_header->p_offset);

	int padding_size =
		next_segment ? (int)(next_segment->p_offset - injection_values->injection_offset) : 0;

	injection_values->padding_size = padding_size > 0 ? padding_size : 0;

	Elf64_Off entry_offset =
		injection_values->injection_addr - elf_header->e_entry;

	Elf64_Off segment_offset =
		injection_values->injection_offset - program_header->p_offset;

	Elf64_Shdr *shdr = ww_get_text_section_header();
	if (!shdr)
		ww_print_error_and_exit(WW_ERR_NOTEXTSEC);

	Elf64_Off text_offset = injection_values->injection_offset - shdr->sh_offset;
	Elf64_Off text_length  = shdr->sh_size;

	patch->main_entry_offset_from_stub   = entry_offset;
	patch->text_segment_offset_from_stub = segment_offset;
	patch->text_section_offset_from_stub = text_offset;
	patch->text_length                   = text_length;

	if (g_modes & WW_VERBOSE)
	{
		printf(WW_GREEN_COLOR);
		printf("╔══════════════════════════════════════════════════════════════╗\n");
		printf("║                 ░░ WOODY INJECTION REPORT ░░                 ║\n");
		printf("╠══════════════════════════════════════════════════════════════╣\n");
		printf("║                  All values are hexadecimal                  ║\n");
		printf("╚══════════════════════════════════════════════════════════════╝\n\n");
		printf(WW_RESET_COLOR);


		printf(WW_YELLOW_COLOR "▶ TARGET METADATA\n" WW_RESET_COLOR);
		printf("   filesize:\t\t0x%lx\n\n", g_file_size);


		printf(WW_YELLOW_COLOR "▶ STUB ENGINE\n" WW_RESET_COLOR);
		printf("   stub size:\t\t0x%lx\n", sizeof_stub);
		printf("   padding size:\t0x%x\n\n", padding_size);


		printf(WW_YELLOW_COLOR "▶ EXECUTABLE SEGMENT\n" WW_RESET_COLOR);
		printf("   p_vaddr:\t\t0x%lx\n", program_header->p_vaddr);
		printf("   p_filesz:\t\t0x%lx\n", program_header->p_filesz);
		printf("   p_offset:\t\t0x%lx\n\n", program_header->p_offset);


		printf(WW_YELLOW_COLOR "▶ INJECTION POINT\n" WW_RESET_COLOR);
		printf("   injection offset:\t0x%lx\n", injection_values->injection_offset);
		printf("   injection addr:\t0x%lx\n\n", injection_values->injection_addr);


		printf(WW_YELLOW_COLOR "▶ ENTRY RESOLUTION\n" WW_RESET_COLOR);
		printf("   original e_entry:\t0x%lx\n", elf_header->e_entry);
		printf("   entry delta:\t\t0x%lx\n\n", entry_offset);


		printf(WW_YELLOW_COLOR "▶ .text MAPPING\n" WW_RESET_COLOR);
		printf("   section offset:\t0x%lx\n", text_offset);
		printf("   segment offset:\t0x%lx\n", segment_offset);
		printf("   section size:\t0x%lx\n\n", text_length);


		printf(WW_RED_COLOR);
		printf("╔══════════════════════════════════════════════════════════════╗\n");
		printf("║               END OF INJECTION DIAGNOSTICS                   ║\n");
		printf("╚══════════════════════════════════════════════════════════════╝\n\n");
		printf(WW_RESET_COLOR);
	}
}

void ww_inject_stub(
	Elf64_Ehdr *elf_header,
	Elf64_Phdr *program_header,
	char *key
)
{
	if (elf_header->e_entry > program_header->p_vaddr + program_header->p_memsz)
	{
		free(key);
		ww_print_error_and_exit(WW_ERR_CORRUPTPHDR);
	}

	ww_t_patch patch;
	ww_t_injection_value injection_values;
	size_t sizeof_stub = sizeof(g_stub);

	injection_values.injection_offset =
		program_header->p_offset + program_header->p_filesz;

	// Get the next LOAD segment to compute the padding between this and
	// the executable LOAD segment
	Elf64_Phdr *next_segment =
		ww_get_next_load_segment_by_offset(
			elf_header,
			program_header,
			injection_values.injection_offset
		);

	if (!next_segment)
		ww_print_error_and_exit(WW_ERR_NEXT_LOAD_SEGMENT_NOT_FOUND);

	bool failed = false;
	// If no mode has been given
	bool no_mode =
		!(g_modes & WW_INJECTREG_PADDING) &&
		!(g_modes & WW_INJECTREG_SHIFT);

	ww_compute_injection_values(
		elf_header, program_header, next_segment,
		&injection_values, &patch
	);

	// -------------------------
	// PADDING MODE
	// -------------------------

	printf(WW_YELLOW_COLOR "▶ PADDING MODE RESOLUTION...\n" WW_RESET_COLOR);

	if (g_modes & WW_INJECTREG_PADDING || no_mode)
	{
		if ((int)sizeof_stub <= injection_values.padding_size)
		{
			g_modes |= WW_INJECTREG_PADDING;

			Elf64_Off rel =
				injection_values.injection_offset - program_header->p_offset;

			elf_header->e_entry = program_header->p_vaddr + rel;

			ww_padding_injection(injection_values.injection_offset, sizeof_stub);
		}
		else if (g_modes & WW_INJECTREG_PADDING)
		{
			ww_print_error_and_exit(WW_ERR_CANNOTINJECTPADDING);
		}
		else
		{
			fprintf(stderr,
				WW_RED_COLOR
				"Cannot inject into padding: not sufficient space.\n"
				WW_RESET_COLOR);

			if (!(g_modes & WW_INJECTREG_SHIFT))
				g_modes |= WW_INJECTREG_SHIFT;

			failed = true;
		}
	}

	// -------------------------
	// SHIFT MODE
	// -------------------------
	if (g_modes & WW_INJECTREG_SHIFT || (failed && no_mode))
	{
		ww_shifting_injection(injection_values.injection_offset, sizeof_stub);

		elf_header = (Elf64_Ehdr *)g_mapped_data;
		program_header = (Elf64_Phdr *)(g_mapped_data + elf_header->e_phoff);

		// Recompute entry safely AFTER layout change
		Elf64_Off rel =
			injection_values.injection_offset - program_header->p_offset;

		elf_header->e_entry = program_header->p_vaddr + rel;
	}

	ww_patch_stub(key, &patch, injection_values.injection_offset, sizeof_stub);
	free(key);
}