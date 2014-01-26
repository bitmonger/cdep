/* Example: parse a simple configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    cdep_init();
    if (ini_parse("deps.ini", handler, NULL) < 0) {
        printf("Can't load 'deps.ini'\n");
        return 1;
    }
    cdep_execute();

    cdep_cleanup();

    return 0;
}
