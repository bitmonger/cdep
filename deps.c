/* cdep -- simple dependency resolver

cdep is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

https://github.com/bitmonger/cdep

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deps.h"

#define NAME_LEN 1024
#define CMD_LEN 1024

int __find_target_by_name(const char *name);
const char* __find_name_by_target(int target);
struct deps_list* __allocate_new_entry(struct deps_list *here);
struct deps_list* __insert_list_entry(struct deps_list *here, struct deps_list *entry);
struct deps_list* __remove_list_entry(struct deps_list *ent);
struct deps_list* __find_target(struct deps_list *start, int target);
void __remove_target(int target);
void __remove_marked();
void __mark_target(int target);
void __store_target_commands(int group, int target);
struct deps_list* __list_tail(struct deps_list* list);
void __print_list();
int __list_length();
int __test_target_has_no_deps(int target);

struct deps_list {
    int target;
    int dep;
    int marked;
    char cmd[CMD_LEN];
    char name[NAME_LEN];
    struct deps_list *next;
};

int gl_nextId = 1;
struct deps_list dl;
struct deps_list cl;

int cdep_init() 
{
    memset(&dl, 0, sizeof(struct deps_list));
    memset(&cl, 0, sizeof(struct deps_list));
    return 0;
}

int cdep_add_target(const char* name)
{
    struct deps_list *c;

    if (__find_target_by_name(name) > 0)
        return -1;

    c = __allocate_new_entry(__list_tail(&dl));
    c->target = gl_nextId++;
    c->dep = 0;
    strncpy(c->name, name, NAME_LEN);
    return c->target;
}

int cdep_add_dependency(const char* name, const char *dep_name)
{
    int target, dep;
    struct deps_list *c;

    target = __find_target_by_name(name);
    if (target == 0)
        target = cdep_add_target(name);

    dep = __find_target_by_name(dep_name);
    if (dep == 0)
        dep = cdep_add_target(dep_name);

    c = __allocate_new_entry(__list_tail(&dl));
    c->target = target;
    c->dep = dep;
    return 0;
}

int cdep_add_command(const char *target_name, const char *command)
{
    int target;
    struct deps_list *c;

    target = __find_target_by_name(target_name);
    if (target == 0)
        target = cdep_add_target(target_name);

    c = __allocate_new_entry(__list_tail(&dl));
    c->target = target;
    strncpy(c->cmd, command, CMD_LEN);
    return 0;
}

int cdep_execute()
{
    int i;
    int len;
    int group = 1;
    struct deps_list *c;

    while ((len = __list_length()))
    {
        for (i = 1; i < gl_nextId; i++)
        {
            c = __find_target(&dl, i);
            if (c == NULL)
                continue;
            
            if (c->marked == 0 && __test_target_has_no_deps(i))
            {
                __store_target_commands(group, i);
                __mark_target(i);
            }
        }
        group++;
        __remove_marked();
        if (len == __list_length())
        {
            printf("Error: Dependencies not solvable\n");
            return -1;
        }
    }
    return 0;
}

int cdep_print_build()
{
    struct deps_list *c = cl.next;

    while (c != NULL)
    {
        printf("%d \"%s\" \"%s\"\n", c->dep, c->name, c->cmd);
        c = c->next;
    }
    return 0;
}

int cdep_cleanup()
{
    struct deps_list *c = dl.next, *cn;
    while (c)
    {
        cn = c->next;
        free(c);
        c = cn;
    }
    c = cl.next;
    while (c)
    {
        cn = c->next;
        free(c);
        c = cn;
    }
    return 0;
}

int __find_target_by_name(const char *name)
{
    struct deps_list *c = &dl;
    while (c != NULL)
    {
        if (strcmp(c->name, name) == 0)
            return c->target;
        c = c->next;
    }
    return 0;
}

const char* __find_name_by_target(int target)
{
    struct deps_list *c = &dl;
    while (c != NULL)
    {
        if (c->target == target)
            return c->name;
        c = c->next;
    }
    return 0;
}

struct deps_list* __allocate_new_entry(struct deps_list *here)
{
    struct deps_list *c = here->next;

    here->next = malloc(sizeof(struct deps_list));
    memset(here->next, 0, sizeof(struct deps_list));
    here->next->next = c;
    return here->next;
}

struct deps_list* __insert_list_entry(struct deps_list *here, struct deps_list *entry)
{
    struct deps_list *c = here->next;

    here->next = entry;
    here->next->next = c;
    return entry;
}

struct deps_list* __remove_list_entry(struct deps_list *ent)
{
    struct deps_list *before = &dl;
    while (before->next && before->next != ent)
    {
        before = before->next;
    }
    before->next = ent->next;
    free(ent);
    return before;
}

struct deps_list* __list_tail(struct deps_list *list)
{
    while (list->next != NULL)
    {
        list = list->next;
    }
    return list;
}

int __list_length()
{
    int i = 0;
    struct deps_list *c = &dl;

    while (c->next)
    {
        i++;
        c = c->next;
    }
    return i;
}

int __test_target_has_no_deps(int target)
{
    struct deps_list *c = &dl;
    while (c)
    {
        if (c->target == target && c->dep != 0)
            return 0;
        c = c->next;
    }
    return 1;
}

struct deps_list* __find_target(struct deps_list *start, int target)
{
    struct deps_list *c = start;
    while (c && c->target != target)
    {
        c = c->next;
    }
    return c;
}

void __remove_target(int target)
{
    struct deps_list *c = &dl;

    while (c->next != NULL)
    {
        c = c->next;
        if (c->dep == target)
            c->dep = 0;
        if (c->target == target)
        {
            c = __remove_list_entry(c);
        }
    }
}

void __remove_marked()
{
    struct deps_list *c = &dl;
    int i;

    for (i=1; i < gl_nextId; i++)
    {
        if ((c = __find_target(&dl, i)))
        {
            if (!c->marked)
                continue;

            __remove_target(c->target);
        }
    }
}

void __mark_target(int target)
{
    struct deps_list *c = &dl;

    while (c->next != NULL)
    {
        c = c->next;
        if (c->target == target)
            c->marked = 1;
    }
}

void __store_target_commands(int group, int target)
{
    struct deps_list *c = &dl;
    struct deps_list *n = &dl;

    while ((c = __find_target(c, target)))
    {
        if (strlen(c->cmd) > 0)
        {
            n = __allocate_new_entry(__list_tail(&cl));
            strncpy(n->cmd, c->cmd, CMD_LEN);
            strncpy(n->name, __find_name_by_target(c->target), NAME_LEN);
            n->dep = group;
            n->target = target;
        }
        c = c->next;
    }
}

void __print_list(struct deps_list *list)
{
    struct deps_list *c = list;
    while (c->next)
    {
        c = c->next;
        printf("[%s]\ntarget = %d\ndep = %d\ncmd = %s\n\n", __find_name_by_target(c->target), c->target, c->dep, c->cmd);
    } 
}

