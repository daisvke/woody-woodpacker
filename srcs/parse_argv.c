#include "ww.h"

/* Get the filename from argv. It should correspond to the only arg
 *  doesn't begin with '-'.
 */ 
char	*ww_get_filename(char *argv[])
{
	char *filename = NULL;
	for (int i = 1; argv[i] != NULL; i++) {
		if (argv[i][0] == '-')
			continue;
		else if (filename == NULL)
			filename = argv[i];

	}
	return filename;
}

// Check if the option given as 'arg' corresponds to the option given as 'opt'
static bool ww_check_opt(char *arg, char *opt)
{
	int opt_len = strlen(opt);
	int arg_len = strlen(arg);

	int len;
	if (opt_len > arg_len)
		len = opt_len;
	else
		len = arg_len;

	if (strncmp(arg, opt, len) == 0)
		return true;
	return false;
}

// Parre the given arguments and activate options 
void ww_parse_argv(char *argv[])
{
	// We begin at index = 1 as index 0 contains the program name
	for (int i = 1; argv[i] != NULL; i++) {
		if (ww_check_opt(argv[i], "--verbose") || ww_check_opt(argv[i], "-v"))
			g_modes |= WW_VERBOSE;
		else if (ww_check_opt(argv[i], "--injection-type=padding") || ww_check_opt(argv[i], "-i=p"))
			g_modes |= WW_INJECTREG_PADDING;
		else if (ww_check_opt(argv[i], "--injection-type=shift") || ww_check_opt(argv[i], "-i=s"))
			g_modes |= WW_INJECTREG_SHIFT;
		else if (ww_check_opt(argv[i], "--shellcode-type=default") || ww_check_opt(argv[i], "-s=d")) {
			g_modes |= WW_SHELLCODE_DEFAULT;
		} else if (ww_check_opt(argv[i], "--shellcode-type=virus") || ww_check_opt(argv[i], "-s=v")) {
			g_modes |= WW_SHELLCODE_VIRUS;
		} else if (argv[i][0] == '-') {
			fprintf(stderr, WW_RED_COLOR "option: %s\n" WW_RESET_COLOR, argv[i]);
			ww_print_error_and_exit(WW_ERR_UNRECOGNIZEDOPT);
		}
	}

	// Default behavior is the padding injection
	if (!(g_modes & WW_INJECTREG_PADDING || g_modes & WW_INJECTREG_SHIFT))
		g_modes |= WW_INJECTREG_PADDING;

	// Set the default shellcode
	if (!(g_modes & WW_SHELLCODE_DEFAULT || g_modes & WW_SHELLCODE_VIRUS))
		g_modes |= WW_SHELLCODE_DEFAULT;
}
