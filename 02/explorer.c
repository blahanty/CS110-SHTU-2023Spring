/* Include essential header files. */
#include "explorer.h"
#include "dir.h"
#include "file.h"
#include "node.h"
/* Include essential header files. */
#include <stdlib.h>
#include <string.h>

struct explorer *explorer_new(void) {
    /* Allocate memory */
    struct explorer *exp = calloc(1, sizeof(struct explorer));
    /* Initialization */
    exp->cwd = exp->root = dir_new("root");
    return exp;
}

void explorer_release(struct explorer *exp) {
    /* Check for null pointer */
    if (!exp) {
        return;
    }
    /* Release the resources */
    dir_release(exp->root);
    free(exp);
}

bool explorer_read(const struct explorer *exp, const char *name, int offset,
                   int bytes, char *buf) {
    /* Initialization */
    struct node *sub = NULL;
    /* Check for null pointer */
    if (!exp) {
        return false;
    }
    /* Check if the file exists */
    sub = dir_find_node(exp->cwd, name);
    if (!sub || sub->is_dir) {
        return false;
    }
    /* Read the file */
    return file_read(sub->inner.file, offset, bytes, buf);
}

bool explorer_write(struct explorer *exp, const char *name, int offset,
                    int bytes, const char *buf) {
    /* Initialization */
    struct node *sub = NULL;
    /* Check for null pointer */
    if (!exp) {
        return false;
    }
    /* Check if the file exists */
    sub = dir_find_node(exp->cwd, name);
    if (!sub || sub->is_dir) {
        return false;
    }
    /* Write the file */
    return file_write(sub->inner.file, offset, bytes, buf);
}

bool explorer_create(struct explorer *exp, char *name, int type) {
    /* Check for valid arguments */
    if (!exp || dir_find_node(exp->cwd, name)) {
        return false;
    }
    /* Add file */
    return dir_add_file(exp->cwd, type, name);
}

bool explorer_mkdir(struct explorer *exp, char *name) {
    /* Check for valid arguments */
    if (!exp || dir_find_node(exp->cwd, name))
        return false;
    /* Add subdir */
    return dir_add_subdir(exp->cwd, name);
}

bool explorer_delete(struct explorer *exp, const char *name) {
    /* Check for null pointer */
    if (!exp)
        return false;
    /* Delete the node */
    return dir_delete(exp->cwd, name);
}

bool explorer_cdpar(struct explorer *exp) {
    /* Check for null pointer */
    /* Check if the parent directory exists. */
    if (!exp || exp->cwd == exp->root)
        return false;
    /* Change directory to the parent directory of current working directory. */
    exp->cwd = exp->cwd->parent;
    return true;
}

bool explorer_chdir(struct explorer *exp, const char *name) {
    /* Initialization */
    struct node *sub = NULL;
    /* Check for null pointer */
    if (!exp || !name)
        return false;
    /* Check if a directory called `name` exists. */
    sub = dir_find_node(exp->cwd, name);
    if (!sub || !sub->is_dir)
        return false;
    /* Change current working directory to a subdir whose name matches with `name`. */
    exp->cwd = sub->inner.dir;
    return true;
}

bool explorer_support_filetype(struct explorer *exp, open_func callback,
                               int filetype) {
    /* Check for null pointer */
    /* Check whether filetype is valid. */
    if (!exp || callback == NULL || filetype < 0 || filetype >= MAX_FT_NO)
        return false;
    /* Check if a callback function for `filetype` exists. */
    if (exp->ft_open[filetype] != NULL)
        return false;
    /* Register a new callback function for filetype number `filetype`. */
    exp->ft_open[filetype] = callback;
    return true;
}

bool explorer_open(const struct explorer *exp, const char *name) {
    /* Initialization */
    struct node *sub = NULL;
    struct file *file = NULL;
    /* Check for null pointer */
    if (!exp || name == NULL)
        return false;
    /* Check if a file called `name` exists. */
    sub = dir_find_node(exp->cwd, name);
    if (!sub || sub->is_dir)
        return false;
    /* Open the file. */
    file = sub->inner.file;
    /* Check if a callback function for the filetype exists. */
    if (!exp->ft_open[file->type])
        return false;
    exp->ft_open[file->type](file);
    return true;
}

/* @function: `search`
 * @parameter: `node: struct node *` Self.
 * @parameter: `path: char **` Path when searching.
 * @parameter: `name: char *` Name to be searched.
 * @parameter: `callback: find_func` The callback function called when we find the file.
 * @return: `void`
 * @description: Search for files with name equal to `name` of the current file or all the descendants of the current directory.
 * If the current node is a file, check if the name of the node is equal to `name`.
 * If the current node is a directory, step into it and continue searching.
 * For every file with name equal to `name`, invoke the callback function with the path relative to the current working node. */
static void search(struct node *node, char **path, const char *name,
                   find_func callback) {
    /* Initialization */
    struct directory *dir = NODE_DIR(node);
    int i = 0, pathlen = strlen(*path);
    /* Reallocate memory */
    *path = realloc(*path, pathlen + strlen(node->name) + 2);
    /* Modify the path */
    strcat(*path, "/");
    strcat(*path, node->name);
    if (node->is_dir) {
        /* The current node is a directory, step into it and continue searching */
        for (i = 0; i < dir->size; i++) {
            search(dir->subordinates[i], path, name, callback);
        }
    }
    /* The current node is a file, check if the name of the node is equal to `name`. */
    else if (strncmp(node->name, name, MAX_NAME_LEN) == 0) {
        callback(*path, NODE_FILE(node));
    }
    /* Reallocate memory */
    *path = realloc(*path, pathlen + 1);
    (*path)[pathlen] = '\0';
}

void explorer_search_recursive(struct explorer *exp, const char *name,
                               find_func callback) {
    /* Initialization */
    char *path = NULL;
    /* Check for null pointer */
    if (!exp || !name || !callback) {
        return;
    }
    /* Allocate memory */
    path = calloc(1, 1);
    /* Search for files with name equal to `name` in all the descendants of the current working directory. */
    search(exp->cwd->base, &path, name, callback);
    /* Release the resources */
    free(path);
}

/* @function: `explorer_contain`
 * @parameter: `exp: struct explorer *` Self.
 * @parameter: `name: char *` Name to be searched.
 * @return: `true` if exp and name is not NULL and a subordinate called `name` exists and `false` otherwise.
 * @description: Search for files with name equal to `name` in all the descendants of the current working directory.*/
bool explorer_contain(struct explorer *exp, const char *name) {
    return exp ? dir_find_node(exp->cwd, name) != NULL : false;
}
