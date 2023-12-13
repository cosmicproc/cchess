#pragma once

typedef struct argument
{
    char *name;
    void (*func_void)(void);
    void (*func_input)(char const *);
} argument;

int argparse(int argc, char const *argv[]);
void invalid(char const *arg);
void help(void);
void version(void);
void resume(char const *filename);