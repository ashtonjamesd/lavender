#include "mem.h"

void *
alloc_function (usize size) {
    void *bytes = malloc(size);

    if (bytes == null) {
        panic("failed to allocate %zu bytes", size);
    }

    return bytes;
}

void *
resize_function (void *ptr, usize new_size) {

    if (new_size == 0) {
        free_function(ptr);
        return null;
    }

    void *bytes = realloc(ptr, new_size);

    if (bytes == null) {
        panic("failed to resize allocation at %p to %zu bytes", ptr, new_size);
    }

    return bytes;
}

void
free_function (void *ptr) {
    free(ptr);
}
