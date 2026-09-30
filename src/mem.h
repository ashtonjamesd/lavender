#ifndef mem_h
#define mem_h

#include "common.h"
#include "concurrency.h"
#include "panic.h"

typedef struct AllocHeader AllocHeader, *AllocHeaderPtr;

#define alignment 16

#define mem_canary_head (0xDEADBEEFDEADBEEFull)
#define mem_canary_tail (0xBAADF00DBAADF00Dull)

// [header] [guard] [obj] [tail]
struct AllocHeader {
    AllocHeaderPtr prev;
    AllocHeaderPtr next;

    const char *file;
    u32 line;
    
    // the size of the allocation
    usize size;
};

void
mem_report (void);

usize 
mem_live_count (void);

usize
mem_live_bytes (void);

bool
mem_verify (ptr x);

ptr
alloc_function (usize size, const char *file, u32 line);

ptr
resize_function (ptr x, usize new_size, const char *file, u32 line);

void
free_function (ptr x, const char *file, u32 line);

#define alloc_bytes(size)      alloc_function(size, __FILE__, __LINE__)
#define alloc(type)            alloc_bytes(sizeof(type))

#define resize(x, new_size)  resize_function(x, new_size, __FILE__, __LINE__)

#define dealloc(x) do { \
        free_function(x, __FILE__, __LINE__); \
        (x) = null; \
    } while (0)

#endif