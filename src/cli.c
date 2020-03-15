#include "cli.h"
#include "argtable3.h"
#include "util.h"

void cli_parse(int argc, char **argv, cli_t *cli)
{
    memset(cli, 0, sizeof(*cli));
    cli->delay = 1;

    struct arg_lit *loop;
    struct arg_int *delay;
    struct arg_lit *help;
    struct arg_lit *version;
    struct arg_end *end;

    void *argtable[] =
    {
        loop = arg_litn("o", "loop", 0, 1, "Enable loop mode."),
        delay = arg_intn("d", "delay", "SECONDS", 0, 1, "Specify loop delay (seconds in range <1..3600>)."),
        version = arg_litn("v", "version", 0, 1, "Show this message and exit."),
        help = arg_litn("h", "help", 0, 1, "Show the version and exit."),
        end = arg_end(20),
    };

    bool usage = false;

    if (arg_parse(argc, argv, argtable) > 0)
        usage = true;

    if (version->count > 0)
    {
        cry("%s, version %s", PROGRAM, VERSION);
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        exit(EXIT_SUCCESS);
    }

    if (help->count > 0)
    {
        fprintf(stderr, "Usage: %s", PROGRAM);
        arg_print_syntax(stderr, argtable, "\n");
        cry("");
        cry("  Environmental Monitoring Toolkit for HARDWARIO Bridge Module.");
        cry("");
        cry("Options:");
        arg_print_glossary(stderr, argtable, "  %-25s %s\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        exit(EXIT_SUCCESS);
    }

    if (loop->count > 0)
        cli->loop = true;

    if (delay->count > 0)
    {
        cli->delay = delay->ival[0];

        if (cli->delay < 1 || cli->delay > 3600 || !cli->loop)
            usage = true;
    }

    if (usage)
    {
        fprintf(stderr, "Usage: %s", PROGRAM);
        arg_print_syntax(stderr, argtable, "\n");
        cry("Try \"%s --help\" for help.", PROGRAM);
        cry("");
        arg_print_errors(stderr, end, PROGRAM);
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        exit(EXIT_FAILURE);
    }

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
}
