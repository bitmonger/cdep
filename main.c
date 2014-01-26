/* cdep -- simple dependency resolver

cdep is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

https://github.com/bitmonger/cdep

*/

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
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[])
{
    struct stat st;
    const char* fname;
    int l;

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
    if ((l = ini_parse(fname, handler, NULL))) {
        printf("%s:%d parse error\n", fname, l);
        return 1;
    }
    if (cdep_execute())
    {
        cdep_cleanup();
        return 0;
    }

    cdep_print_script();
    cdep_cleanup();

    return 0;
}
