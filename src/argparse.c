#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aliases.h"
#include "info.h"
#include "cchess.h"
#include "argparse.h"

const argument args[] = {
    {"--help", help, NULL},
    {"--version", version, NULL},
    {"-h", help, NULL},
    {"-v", version, NULL},
    {"-r", NULL, resume},
    {"--resume", NULL, resume},
    {NULL, NULL},
};

team first_player = WHITE;

int argparse(int argc, char const *argv[])
{
    while (*++argv)
    {
        bool is_valid = false;
        for (size_t i = 0; args[i].name; i++)
        {
            if (str_equal(*argv, args[i].name))
            {
                if (args[i].func_input)
                    args[i].func_input(*++argv);
                else
                    args[i].func_void();
                is_valid = true;
            }
        }
        if (!is_valid)
            invalid(*argv);
    }
    start_game(first_player);
    return 0;
}

void invalid(char const *arg)
{
    printf("%s: unknown option '%s'.\n", PROGRAM_NAME, arg);
    help();
    exit(-1);
}

void help(void)
{
    printf("usage: %s ", PROGRAM_NAME);
    printf("type 'save' instead of a move to save the current game to a file.");
    for (size_t i = 0; args[i].name; i++)
        printf("[%s] ", args[i].name);
    printf("\n");
    exit(0);
}

void version(void)
{
    printf("%s (%.2f) by %s\n%s (%s)\n", PROGRAM_NAME, VERSION, AUTHOR, LICENSE, WEBSIZE);
    exit(0);
}

void resume(char const *filename)
{
    FILE *file = fopen(filename, "r");
    char next_player[6];
    if (!file || load_board(file) != 0 || !fgets(next_player, 6, file))
    {
        printf("Couldn't load the file.\n");
        return;
    }
    if (str_equal(next_player, "BLACK"))
        first_player = BLACK; 
}