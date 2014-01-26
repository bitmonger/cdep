/* cdep -- simple dependency resolver

cdep is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

https://github.com/bitmonger/cdep

*/
#ifndef __DEPS_H_INCLUDED__
#define __DEPS_H_INCLUDED__


int cdep_init();
int cdep_add_target(const char* name);
int cdep_add_dependency(const char* name, const char *dep_name);
int cdep_add_command(const char *target_name, const char *command);
int cdep_execute();
int cdep_print_script();
int cdep_cleanup();


#endif
