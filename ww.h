#ifndef WOODY_WOODPACKER_H
# define  WOODY_WOODPACKER_H

# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdint.h>
# include <sys/mman.h>		// For mapping 
# include <elf.h>			// For ELF header
# include <stdbool.h>

# include "errors.h"

/*  Colors */
# define _WW_RED_COLOR		"\033[31m"
# define _WW_GREEN_COLOR	"\033[32m"
# define _WW_YELLOW_COLOR	"\033[33m"
# define _WW_RESET_COLOR	"\033[0m"

/* Return */
# define _WW_ERROR		1

/* ELF header */
enum	e_elf_header
{
	_WW_EI_CLASS    = 4,
	_WW_ELFCLASS64  = 2
};

#endif
