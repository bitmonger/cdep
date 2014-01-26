#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct target_node {
    int target;
    int dep;
    char name[255];
    struct target_node *next;
};

void initialize_list(struct target_node *list)
{
    memset(list, 0, sizeof(*list));
    list->next = NULL;
}

void name_target(struct target_node *list, const char* name, int target)
{
    struct target_node *c = list;
    while (c->next != NULL) { 
        c = c->next;
    }

    c->next = malloc(sizeof(struct target_node));
    memset(c->next, 0, sizeof(struct target_node));
    strncpy(c->next->name, name, 255);
    c->next->target = target;
    c->next->dep = -1;
}

const char* get_target_name(struct target_node *list, int target)
{
    struct target_node *c = list;
    while (c != NULL) { 
        if (c->target == target && c->name)
            return c->name;
        c = c->next;
    }
    return "";
}

void define_dep(struct target_node *list, int target, int dep)
{
    struct target_node *c = list;
    struct target_node *new_dep;
    int found = 0;
    while (c->next != NULL) { 
        if (c->target == dep)
            found = 1;
        c = c->next;
    }

    c->next = malloc(sizeof(struct target_node));
    memset(c->next, 0, sizeof(struct target_node));
    c->next->target = target;
    c->next->dep = dep;

    c = c->next;

    if (!found)
    {
        c->next = malloc(sizeof(struct target_node));
        memset(c->next, 0, sizeof(struct target_node));
        c->next->target = dep;
        c->next->dep = -1;
    }
}

void print_list(struct target_node *list)
{
    struct target_node *c = list;
    while (c->next)
    {
        c = c->next;
        printf("name: %s\n target: %d\n dep: %d\n\n", c->name, c->target, c->dep);
    } 
}

void remove_target(struct target_node *list, int target)
{
    struct target_node *c = list;
    struct target_node *next;
    while (c->next)
    {
        next = c->next;
        if (next->target == target)
        {
            c->next = next->next;
            free(next);
        }
        else
        {
            if (next->dep == target)
            {
                next->dep = -1;
            }
            c = c->next;
        }
    }
}

void free_list(struct target_node *list)
{
    struct target_node *c = list;
    struct target_node *next;
    while (c->next)
    {
        next = c->next;
        c->next = c->next->next;
        free(next);
    }
}

int list_length(struct target_node *list)
{
    int i = 0;
    while (list->next)
    {
        i++;
        list = list->next;
    }
    return i;
}

void print_build_order(struct target_node *list)
{
    struct target_node *c = list;
    struct target_node *i = list;
    struct target_node *next;
    int found;

    while (c && c->next)
    {
        next = c->next;
        if (next->dep == -1)
        {
            i = list;
            found = 1;
            while (i->next)
            {
                if (i->next->target == next->target
                    && i->next->dep != -1)
                {
                    found=0;
                    break;
                }
                i = i->next;
            }
            if (found)
            {
                printf("Build: %s (%d)\n", get_target_name(list, next->target), next->target);
                remove_target(list, next->target);
                c = list;
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
    if (list_length(list))
    {
        printf("Error: Dependencies not solvable\n");
    }
}

int main()
{
    int len;
    struct target_node list;
    struct target_node *c;
    initialize_list(&list);

    name_target(&list, "toolchain", 1);
    name_target(&list, "kernel", 2);
    name_target(&list, "bootloader", 3);
    name_target(&list, "userspace", 4);

    define_dep(&list, 4, 3);
    define_dep(&list, 4, 2);
    define_dep(&list, 2, 1);
    define_dep(&list, 3, 1);

//    print_list(&list);

    print_build_order(&list);
    free_list(&list);

    return 0;
}

