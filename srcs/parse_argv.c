#include "ww.h"
#include <getopt.h>

// Get the filename from argv
char *ww_get_filename(int argc, char *argv[])
{
	if (argc - optind != 1)
	{
		fprintf(stderr,
			WW_RED_COLOR
			"Wrong number of arguments\n"
			WW_RESET_COLOR);
		return NULL;
	}

	return argv[optind];
}

void    ww_print_usage(void)
{
    printf(
        "\nUsage:\n\n"
        "./woody_woodpacker [OPTIONS]\n\n"

        "Options:\n"
        "  -v, --verbose                  Enable verbose mode\n"
        "  -i, --injection-type=TYPE      Set injection type\n"
        "                                 TYPE can be:\n"
        "                                   padding (p)\n"
        "                                   shift   (s)\n"
        "  -h, --help                     Display this help message\n"
    );
}

void    ww_parse_argv(int argc, char *argv[])
{
    const char              *short_opts = "vhi:";
    const struct option     long_opts[] = {
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"injection-type", required_argument, NULL, 'i'},
        {NULL, 0, NULL, 0}
    };

    int opt;

    while ((opt = getopt_long(argc, argv,
        short_opts, long_opts, NULL)) != -1)
    {
        switch (opt)
        {
            case 'v':
                g_modes |= WW_VERBOSE;
                break;

            case 'i':
                if (strcmp(optarg, "padding") == 0
                    || strcmp(optarg, "p") == 0)
                {
                    g_modes |= WW_INJECTREG_PADDING;
                }
                else if (strcmp(optarg, "shift") == 0
                    || strcmp(optarg, "s") == 0)
                {
                    g_modes |= WW_INJECTREG_SHIFT;
                }
                else
                {
                    fprintf(stderr,
                        WW_RED_COLOR
                        "Invalid injection type: %s\n"
                        WW_RESET_COLOR,
                        optarg);
                    ww_print_error_and_exit(WW_ERR_UNRECOGNIZEDOPT);
                }
                break;

            case 'h':
                ww_print_usage();
                exit(EXIT_SUCCESS);

            default:
                fprintf(stderr,
                    WW_RED_COLOR
                    "Invalid option\n"
                    WW_RESET_COLOR);
                ww_print_error_and_exit(WW_ERR_UNRECOGNIZEDOPT);
        }
    }
}
