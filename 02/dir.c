/* Include essential header files. */
#include "dir.h"
#include "explorer.h"
#include "file.h"
#include "node.h"
/* Include essential header files. */
#include <stdlib.h>
#include <string.h>

/* static bool dir_add_sub(struct directory *dirnode, struct node *sub); */

struct directory *dir_new(char *name) {
    /* Initialization */
    struct directory *dir = NULL;
    /* Check for null pointer */
    if (!name) {
        return NULL;
    }
    /* Allocate memory */
    dir = calloc(1, sizeof(struct directory));
    /* Initialization */
    dir->capacity = DEFAULT_DIR_SIZE;
    dir->subordinates = calloc(dir->capacity, sizeof(struct node));
    dir->parent = NULL;
    /* Create base node */
    dir->base = node_new(true, name, dir);
    return dir;
}

#include <stdio.h>

void dir_release(struct directory *dir) {
    /* Initialization */
    int i = 0;
    if (!dir) {
        return;
    }
    /* Release all the subordinates */
    for (i = 0; i < dir->size; i++) {
        node_release(dir->subordinates[i]);
    }
    /* Release the resources */
    /* Check if base has already been released. Prevent circular call. */
    if (dir->base) {
        dir->base->inner.dir = NULL;
        node_release(dir->base);
    }
    /* Release data and self. */
    free(dir->subordinates);
    free(dir);
}

struct node *dir_find_node(const struct directory *dir, const char *name) {
    /* Check for null pointer */
    if (!dir || !name)
        return NULL;
    /* Find a subordinate node in `dir` with exactly matched `name`. */
    for (int i = 0; i < dir->size; ++i)
        if (!strcmp(dir->subordinates[i]->name, name))
            /* Read the file */
            return dir->subordinates[i];
    /* The node does not exist. */
    return NULL;
}

bool dir_add_file(struct directory *dir, int type, char *name) {
    /* Check for null pointer */
    if (!dir || !name)
        return false;
    /* Check if a subordinate called `name` already exists. */
    if (dir_find_node(dir, name))
        return false;
    /* Initialize the file. */
    struct file *file = file_new(type, name);
    /* Enlarge the capacity to `2 * current_capacity` if `size` is to exceed `capacity`. */
    if (dir->size >= dir->capacity) {
        dir->capacity *= 2;
        /* Reallocate memory */
        dir->subordinates = realloc(dir->subordinates, dir->capacity * sizeof(struct node));
    }
    /* Make `file` a subordinate of `dir`. */
    dir->subordinates[dir->size++] = file->base;
    return true;
}

bool dir_add_subdir(struct directory *dir, char *name) {
    /* Check for null pointer */
    if (!dir || !name)
        return false;
    /* Check if a subordinate called `name` already exists. */
    if (dir_find_node(dir, name))
        return false;
    /* Initialize the directory. */
    struct directory *sub = dir_new(name);
    /* Set the parent. */
    sub->parent = dir;
    /* Enlarge the capacity to `2 * current_capacity` if `size` is to exceed `capacity`. */
    if (dir->size + 1 >= dir->capacity) {
        dir->capacity *= 2;
        /* Reallocate memory */
        dir->subordinates = realloc(dir->subordinates, dir->capacity * sizeof(struct node));
    }
    /* Make `sub` a subordinate of `dir`. */
    dir->subordinates[dir->size++] = sub->base;
    return true;
}

bool dir_delete(struct directory *dir, const char *name) {
    /* Check for null pointer */
    if (!dir || !name)
        return false;
    /* Check if a subordinate called `name` exists. */
    for (int i = 0; i < dir->size; ++i)
        /* Remove and release the subordinate of `dir` with name equal to `name`. */
        if (!strcmp(dir->subordinates[i]->name, name)) {
            /* Release the resources */
            node_release(dir->subordinates[i]);
            /* Manage the subordinate list. */
            dir->subordinates[i] = dir->subordinates[--dir->size];
            dir->subordinates[dir->size] = NULL;
            return true;
        }
    /* The directory does not exist. */
    return false;
}
