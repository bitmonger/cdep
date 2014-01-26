#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deps.h"

#define NAME_LEN 1024
#define CMD_LEN 1024

int __find_target_by_name(const char *name);
const char* __find_name_by_target(int target);
struct deps_list* __insert_list_entry(struct deps_list *here);
void __remove_list_entry(struct deps_list *ent);
struct deps_list* __find_target(struct deps_list *start, int target);
void __remove_target(int target);
void __print_target_commands(int target);
struct deps_list* __list_tail();
void __print_list();
int __list_length();

struct deps_list {
    int target;
    int dep;
    char cmd[CMD_LEN];
    char name[NAME_LEN];
    struct deps_list *next;
};

int gl_nextId = 1;
struct deps_list dl;

int cdep_init() 
{
    memset(&dl, 0, sizeof(struct deps_list));
    return 0;
}

int cdep_add_target(const char* name)
{
    struct deps_list *c;

    if (__find_target_by_name(name) > 0)
        return -1;

    c = __insert_list_entry(__list_tail());
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

    c = __insert_list_entry(__list_tail());
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

    c = __insert_list_entry(__list_tail());
    c->target = target;
    strncpy(c->cmd, command, CMD_LEN);
    return 0;
}

int cdep_execute()
{
    struct deps_list *c = &dl;
    struct deps_list *i = &dl;
    struct deps_list *next;
    int found;

    while (c && c->next)
    {
        next = c->next;
        if (next->dep == 0)
        {
            i = &dl;
            found = 1;
            while (i->next)
            {
                if (i->next->target == next->target
                    && i->next->dep != 0)
                {
                    found=0;
                    break;
                }
                i = i->next;
            }
            if (found)
            {
                __print_target_commands(next->target);
                __remove_target(next->target);
                c = &dl;
            }
            else
            {
                c = next;
            }
        }
        else
        {
            c = next;
        }
    }
    if (__list_length())
    {
        printf("Error: Dependencies not solvable\n");
        return -1;
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

struct deps_list* __insert_list_entry(struct deps_list *here)
{
    struct deps_list *c = here->next;

    here->next = malloc(sizeof(struct deps_list));
    memset(here->next, 0, sizeof(struct deps_list));
    here->next->next = c;
    return here->next;
}

void __remove_list_entry(struct deps_list *ent)
{
    struct deps_list *before = &dl;
    while (before->next && before->next != ent)
    {
        before = before->next;
    }
    before->next = ent->next;
    free(ent);
}

struct deps_list* __list_tail()
{
    struct deps_list *c = &dl;
    while (c->next != NULL)
    {
        c = c->next;
    }
    return c;
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
            __remove_list_entry(c);
    }
}

void __print_target_commands(int target)
{
    struct deps_list *c = &dl;
    const char *name = __find_name_by_target(target);

    while ((c = __find_target(c, target)))
    {
        if (strlen(c->cmd) > 0)
            printf("%s, %s\n", name, c->cmd);
        c = c->next;
    }
}

void __print_list()
{
    struct deps_list *c = &dl;
    while (c->next)
    {
        c = c->next;
        printf("[%s]\ntarget = %d\ndep = %d\ncmd = %s\n\n", __find_name_by_target(c->target), c->target, c->dep, c->cmd);
    } 
}

