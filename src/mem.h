#ifndef mem_h
#define mem_h

#include "common.h"

ptr
alloc_function (usize size);

ptr
resize_function (void *ptr, usize new_size);

void
free_function (void *ptr);

#define alloc_bytes(size)      alloc_function(size)
#define alloc(type)            alloc_bytes(sizeof(type))

#define resize(ptr, new_size)  resize_function(ptr, new_size)

#define dealloc(ptr) do { \
            free_function(ptr); \
            (ptr) = null; \
        } while (0)

#endif