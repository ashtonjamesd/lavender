#include "mem.h"

static usize live_allocation_count = 0;
static usize live_bytes_allocated = 0;

static pthread_mutex_t memory_lock = PTHREAD_MUTEX_INITIALIZER;

static AllocHeaderPtr live_allocations = null;

void
mem_init () {
    
    assert(live_allocation_count == 0);
    assert(live_bytes_allocated == 0);

    atexit(mem_report);
}

void
mem_report () {
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
alloc_header_from_object (void *ptr) {

    return (AllocHeaderPtr) ((bytePtr)ptr - header_bytes());
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

void *
alloc_function (usize size, const char *file, u32 line) {

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
    allocation->isFreed = true;

    with_mutex (&memory_lock) {

        untrack_allocation(allocation);

        live_allocation_count -= 1;
        live_bytes_allocated -= allocation->size;
    }

    free(allocation);
}
