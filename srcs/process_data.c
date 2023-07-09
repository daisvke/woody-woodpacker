#include "ww.h"

void elf_print_header(Elf64_Ehdr * data)
{
  printf("Point d'entree                               : %p\n", data->e_entry);
  printf("Adresse de la table des segments (relative)  : + %4d bytes\n", data->e_phoff);
  printf("Nombre d'elements dans la table des segments : %d\n", data->e_phnum);
  printf("Adresse de la table des segments (relative)  : + %4d bytes\n", data->e_shoff);
  printf("Nombre d'elements dans la table des sections : %d\n", data->e_shnum);
}

void elf_print_sections_name(Elf64_Ehdr * data)
{
  int i;

  // on recupere un pointeur sur le premier header de section
  // ne as oublier de caster en void*, autrement l'arithmetique entre les pointeurs ne sera pas bon
  Elf64_Shdr *section_header_start = (Elf64_Shdr*)((void*)data + data->e_shoff);

  // on retrouve la string table avec le nom des sections ..
  Elf64_Shdr sections_string = section_header_start[data->e_shstrndx];
  
  // on recupere un pointeur sur le debut de la section et donc sur la premiere chaine
  char *strings = (char*)((void*)data + sections_string.sh_offset);

  // pour stoquer chaque section
  Elf64_Shdr section_header;

  for (i = 0; i < data->e_shnum; i++)
    {
      // on recupere le header de section courrant
      section_header = section_header_start[i];

      // on affiche son nom a partir de la section contenant les strings et son index
      printf("%s\n", strings + section_header.sh_name);
    }
}

Elf64_Shdr *get_section_header(void *f, int idx) {
	return f + (((Elf64_Ehdr *)f)->e_shoff + (idx * ((Elf64_Ehdr *)f)->e_shentsize ));
}


Elf64_Shdr *get_text_section_header() {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)_buffer;

	/* Get the section header of the section name string table
	 * We get this section to know the name of each section header
	 * while we will iterate over them.
	*/
	Elf64_Shdr *strtab = get_section_header(_buffer, ehdr->e_shstrndx);

	/* We iterate over each section header to find .text section name */
	for (size_t i = 1; i < ehdr->e_shnum; i++) {
		Elf64_Shdr *shdr = get_section_header(_buffer, i);
		char *sh_name = (char *)(_buffer + strtab->sh_offset + shdr->sh_name);
		if (strcmp(sh_name, ".text") == 0)
			return shdr;
	}
	return NULL;
}

void	_ww_process_mapped_data()
{
    Elf64_Ehdr *data = (Elf64_Ehdr *)_buffer;
    elf_print_header(data);
    elf_print_sections_name(data);

    char*	_key = _ww_keygen(_WW_KEYCHARSET, _WW_KEYSTRENGTH);
    printf("Generated key: %s\n", _key);
	Elf64_Shdr *txt_shdr = get_text_section_header();
	if (!txt_shdr)
		return;
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer + txt_shdr->sh_offset, txt_shdr->sh_size);
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer + txt_shdr->sh_offset, txt_shdr->sh_size);
    free(_key);
}