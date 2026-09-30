#include "mem.h"

#define alignment 16
static inline usize
aligned_bytes_required (usize n) {
    return (n + (alignment - n % alignment) % alignment);
}

static inline usize
header_bytes (void) {
    return aligned_bytes_required(sizeof(AllocHeader));
}

static inline AllocHeaderPtr
alloc_header_from_object (void *ptr) {
    return (AllocHeaderPtr) ((bytePtr)ptr - header_bytes());
}

void *
alloc_function (usize size, const char *file, u32 line) {

    usize header_size = header_bytes();
    usize total_size = header_size + size;

    if (header_size % alignment != 0) {
        panic("alignment fault");
    }

    AllocHeaderPtr allocation = (AllocHeaderPtr) malloc(total_size);
    if (allocation == null) {
        panic("failed to allocate %zu bytes", size);
    }

    allocation->file = file;
    allocation->line = line;
    allocation->size = size;

    void *object_ptr = (bytePtr)allocation + header_size;
    return object_ptr;
}



void *
resize_function (void *ptr, usize new_size, const char *file, u32 line) {

    if (ptr == null) {
        return alloc_function(new_size, file, line);
    }

    if (new_size == 0) {
        free_function(ptr, file, line);
        return null;
    }

    AllocHeaderPtr allocation = alloc_header_from_object(ptr);
    AllocHeaderPtr resized = realloc(allocation, header_bytes() + new_size);

    if (resized == null) {
        panic("%s:%u failed to resize allocation to %zu bytes", file, line, new_size);
    }

    resized->size = new_size;

    return (bytePtr)resized + header_bytes();
}

void
free_function (void *ptr, const char *file, u32 line) {
    (void)file;
    (void)line;

    if (ptr == null) {
        return;
    }

    AllocHeaderPtr allocation = alloc_header_from_object(ptr);
    free(allocation);
}
