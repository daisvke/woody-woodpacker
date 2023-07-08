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

void	_ww_process_mapped_data()
{
    Elf64_Ehdr *data = (Elf64_Ehdr *)_buffer;
    elf_print_header(data);
    elf_print_sections_name(data);

    char*	_key = _ww_keygen(_WW_KEYCHARSET, _WW_KEYSTRENGTH);
    printf("Generated key: %s\n", _key);
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer + data->e_shoff, _file_size);
    xor_encrypt_decrypt(_key, _WW_KEYSTRENGTH, _buffer + data->e_shoff, _file_size);
    free(_key);
}