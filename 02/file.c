/* Include essential header files. */
#include "file.h"
#include "explorer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file *file_new(int type, char *name) {
    /* Initialization */
    struct file *file = NULL;
    /* Check for integrity */
    if (!name || type < 0 || type >= MAX_FT_NO) {
        return NULL;
    }
    /* Allocate memory and initialize the file. */
    file = calloc(1, sizeof(struct file));
    file->type = type;
    file->size = DEFAULT_FILE_SIZE;
    file->data = calloc(file->size, 1);
    /* Create associate node and set it to base. */
    file->base = node_new(false, name, file);
    return file;
}

void file_release(struct file *file) {
    /* Check for null pointer. */
    if (!file) {
        return;
    }
    /* release the data. */
    /* Check if base has already been released. Prevent circular call. */
    if (file->base) {
        file->base->inner.file = NULL;
        node_release(file->base);
    }
    /* Release the resources. */
    free(file->data);
    free(file);
}

bool file_write(struct file *file, int offset, int bytes, const char *buf) {
    /* Check for null pointer */
    /* Check whether offset is negative. */
    if (!file || !buf || offset < 0)
        return false;
    /* Enlarge the file size if `offset + bytes` exceed current file size. */
    if (offset + bytes > file->size) {
        file->size = offset + bytes;
        file->data = realloc(file->data, file->size);
    }
    /* overwrite the file data within the range from `offset` to `offset + bytes` with the data from `buf`. */
    for (int i = 0; i < bytes; ++i)
        file->data[offset + i] = buf[i];
    return true;
}

bool file_read(const struct file *file, int offset, int bytes, char *buf) {
    /* Check for null pointer */
    /* Check whether offset is negative. */
    /* Check whether `offset + bytes` exceed current file size. */
    if (!file || offset < 0 || offset + bytes > file->size)
        return false;
    /* Read the file data within the range from `offset` to `offset + bytes` to `buf`.*/
    for (int i = 0; i < bytes; ++i)
        buf[i] = file->data[offset + i];
    return true;
}
