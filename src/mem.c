#include "mem.h"

#ifndef MEM_NO_TRACK_LEAKS

static usize live_allocation_count = 0;
static usize live_bytes_allocated = 0;

static pthread_mutex_t memory_lock = PTHREAD_MUTEX_INITIALIZER;

static AllocHeaderPtr live_allocations = null;

static pthread_once_t mem_initialised = PTHREAD_ONCE_INIT;

#define head_guard_value 0xA110CA7EDA110CA7ull
#define freed_guard_value 0xF4EEDF4EEDF4EED0ull

static void
mem_setup (void) {

    assert(live_allocation_count == 0);
    assert(live_bytes_allocated == 0);

    atexit(mem_report);
}

void
mem_init (void) {

    pthread_once(&mem_initialised, mem_setup);
}

void
mem_report (void) {
    with_mutex (&memory_lock) {

        bool has_allocations = live_allocation_count > 0;

        if (has_allocations) {
            fprintf(
                stderr, "memory leaks detected: %zu allocations, %zu bytes\n",
                live_allocation_count, live_bytes_allocated
            );
        }

        for (AllocHeaderPtr a = live_allocations; a != null; a = a->next) {
            fprintf(stderr, "    %zu bytes allocated at %s:%u\n", a->size, a->file, a->line);
        }
        
        if (has_allocations) {
            printf("\n");
        }
    }
}

usize 
mem_live_count (void) {

    usize n;
    with_mutex (&memory_lock) {

        n = live_allocation_count;
    }

    return n;
}

usize
mem_live_bytes (void) {

    usize n;
    with_mutex (&memory_lock) {

        n = live_bytes_allocated;
    }

    return n;
}

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
alloc_header_from_object (ptr x) {

    return (AllocHeaderPtr) ((bytePtr)x - header_bytes());
}

static void 
track_allocation (AllocHeaderPtr allocation) {

    allocation->prev = null;
    allocation->next = live_allocations;

    if (live_allocations != null) {
        live_allocations->prev = allocation;
    }

    live_allocations = allocation;
}

static void
untrack_allocation (AllocHeaderPtr allocation) {

    if (allocation->prev != null) {
        allocation->prev->next = allocation->next;
    } else {
        live_allocations = allocation->next;
    }

    if (allocation->next != null) {
        allocation->next->prev = allocation->prev;
    }
}

ptr
alloc_function (usize size, const char *file, u32 line) {

    mem_init();

    usize header_size = header_bytes();
    usize total_size = header_size + size;

    if (header_size % alignment != 0) {
        panic("alignment fault");
    }

    AllocHeaderPtr allocation = (AllocHeaderPtr) malloc(total_size);
    if (allocation == null) {
        panic("%s:%u failed to allocate %zu bytes", file, line, size);
    }

    allocation->file = file;
    allocation->line = line;
    allocation->size = size;
    allocation->isFreed = false;

    with_mutex (&memory_lock) {

        track_allocation(allocation);

        live_allocation_count +=1;
        live_bytes_allocated += size;
    }

    return (ptr)((bytePtr)allocation + header_size);
}

ptr
resize_function (ptr x, usize new_size, const char *file, u32 line) {

    if (x == null) {
        return alloc_function(new_size, file, line);
    }

    if (new_size == 0) {
        free_function(x, file, line);
        return null;
    }

    AllocHeaderPtr allocation = alloc_header_from_object(x);

    with_mutex (&memory_lock) {

        untrack_allocation(allocation);
    }

    AllocHeaderPtr resized = realloc(allocation, header_bytes() + new_size);

    if (resized == null) {
        panic("%s:%u failed to resize allocation to %zu bytes", file, line, new_size);
    }

    with_mutex (&memory_lock) {

        live_bytes_allocated -= resized->size;
        live_bytes_allocated += new_size;

        track_allocation(resized);
    }

    resized->size = new_size;

    return (ptr)((bytePtr)resized + header_bytes());
}

void
free_function (ptr x, const char *file, u32 line) {
    
    (void)file;
    (void)line;

    if (x == null) {
        return;
    }

    AllocHeaderPtr allocation = alloc_header_from_object(x);
    allocation->isFreed = true;

    with_mutex (&memory_lock) {

        untrack_allocation(allocation);

        live_allocation_count -= 1;
        live_bytes_allocated -= allocation->size;
    }

    free(allocation);
}

#else

void
mem_init (void) {
}

void
mem_report (void) {
}

usize
mem_live_count (void) {

    return 0;
}

usize
mem_live_bytes (void) {

    return 0;
}

ptr 
alloc_function (usize size, const char *file, u32 line) {

    ptr bytes = malloc(size);

    if (bytes == null) {
        panic("%s:%u failed to allocate %zu bytes", file, line, size);
    }

    return bytes;
}

ptr 
resize_function (ptr ptr, usize new_size, const char *file, u32 line) {

    if (new_size == 0) {
        free_function(ptr, file, line);
        return null;
    }

    ptr bytes = realloc(ptr, new_size);

    if (bytes == null) {
        panic("%s:%u failed to resize allocation to %zu bytes", file, line, new_size);
    }

    return bytes;
}

void
free_function (ptr ptr, const char *file, u32 line) {

    (void)file;
    (void)line;

    free(ptr);
}

#endif
