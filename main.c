/* Example: parse a simple configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "ini.h"
#include "deps.h"

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    cdep_add_target(section);
    if (strcmp(name, "cmd") == 0)
    {
        cdep_add_command(section, value);
    }
    else if (strcmp(name, "dep") == 0)
    {
        cdep_add_dependency(section, value);
    }
    else
    {
        printf("Unknown identifier:\n[%s]\n%s = %s\n", section, name, value);
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[])
{
    struct stat st;
    const char* fname;

    if (argc < 2)
        fname = "deps.ini";
    else
        fname = argv[1];

    if (stat(fname, &st))
    {
        perror(fname);
        return -1;
    }

    cdep_init();
    if (ini_parse(fname, handler, NULL) < 0) {
        printf("Can't load %s\n", fname);
        return 1;
    }
    cdep_execute();

    cdep_cleanup();

    return 0;
}
