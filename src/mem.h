#ifndef mem_h
#define mem_h

#include "common.h"
#include "concurrency.h"

typedef struct AllocHeader AllocHeader, *AllocHeaderPtr;

struct AllocHeader {
    AllocHeaderPtr prev;
    AllocHeaderPtr next;

    const char *file;
    u32 line;
    
    // the size of the allocation
    usize size;
    bool isFreed;
};

void
mem_report (void);

usize 
mem_live_count (void);

usize
mem_live_bytes (void);

ptr
alloc_function (usize size, const char *file, u32 line);

ptr
resize_function (void *ptr, usize new_size, const char *file, u32 line);

void
free_function (void *ptr, const char *file, u32 line);

#define alloc_bytes(size)      alloc_function(size, __FILE__, __LINE__)
#define alloc(type)            alloc_bytes(sizeof(type))

#define resize(ptr, new_size)  resize_function(ptr, new_size, __FILE__, __LINE__)

#define dealloc(ptr) do { \
        free_function(ptr, __FILE__, __LINE__); \
        (ptr) = null; \
    } while (0)

#endif