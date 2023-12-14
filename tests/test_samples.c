#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void fail(void);

int main(int argc, char const *argv[])
{
    struct dirent *dp;
    DIR *samples;
    if ((samples = opendir("samples")) == NULL)
        fail();

    while ((dp = readdir(samples)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        FILE *process = popen("../cchess", "w");
        if (process == NULL)
            fail();
        char *path = malloc(sizeof(dp->d_name) + sizeof("samples/"));
        strlcpy(path, "samples/", sizeof(dp->d_name) + sizeof("samples/"));
        strlcat(path, dp->d_name, sizeof(dp->d_name) + sizeof("samples/"));
        printf("\nTesting sampe: %s\n", path);
        FILE *sample = fopen(path, "r");
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), sample))
        {
            if (buffer[strlen(buffer) - 1] != '\n')
            {
                printf("Lines are too long. Couldn't read the file.");
                fail();
            }
            if (buffer[0] != '#')
                fwrite(buffer, 1, strlen(buffer), process);
        }

        printf("\nExit code: %d\n", pclose(process));
    }
    return 0;
}

void fail(void)
{
    printf("Test failed.");
    exit(-1);
}