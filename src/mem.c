#include "mem.h"

#ifndef MEM_NO_TRACK_LEAKS

static usize live_allocation_count = 0;
static usize live_bytes_allocated = 0;

static pthread_mutex_t memory_lock = PTHREAD_MUTEX_INITIALIZER;

static AllocHeaderPtr live_allocations = null;

static pthread_once_t mem_initialised = PTHREAD_ONCE_INIT;

static void
mem_setup (void) {

    assert(live_allocation_count == 0);
    assert(live_bytes_allocated == 0);

    atexit(mem_report);
}

static void
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

static inline usize
aligned_bytes_required (usize n) {

    return (n + (alignment - n % alignment) % alignment);
}

static inline usize
header_bytes (void) {

    const usize guard_size = sizeof(mem_canary_head);
    const usize header_size = sizeof(AllocHeader);

    const usize size 
        = guard_size + header_size;
    
    return aligned_bytes_required(size);
}

static inline AllocHeaderPtr
alloc_header_from_object (ptr x) {

    return (AllocHeaderPtr) ((bytePtr)x - header_bytes());
}

// the guard sits directly before the object, irrespective of the header size
static inline u64Ptr
guard_of (ptr x) {

    return (u64Ptr)x - 1;
}

// the object size rounded up to 8, so the tail after it can be read as a u64
static inline usize
tail_offset (usize size) {

    return (size + sizeof(u64) - 1) / sizeof(u64) * sizeof(u64);
}

static inline u64Ptr
tail_of (ptr x) {
    const AllocHeaderPtr header = alloc_header_from_object(x);

    return (u64Ptr)((bytePtr)x + tail_offset(header->size));
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

bool
mem_verify (ptr x) {
    
    if (x == null) {
        return true;
    }

    bool guard_ok = *guard_of(x) == mem_canary_head;
    bool tail_ok = *tail_of(x) == mem_canary_tail;

    return guard_ok and tail_ok;
}

ptr
alloc_function (usize size, const char *file, u32 line) {

    mem_init();

    const usize header_size = header_bytes();
    const usize tail_size = sizeof(mem_canary_tail) + tail_offset(size);
    
    const usize total_size = header_size + tail_size;

    AllocHeaderPtr allocation = (AllocHeaderPtr) malloc(total_size);
    if (allocation == null) {
        panic("%s:%u failed to allocate %zu bytes", file, line, size);
    }

    allocation->file = file;
    allocation->line = line;
    allocation->size = size;

    ptr object = (bytePtr)allocation + header_size;
    
    *guard_of(object) = mem_canary_head;
    *tail_of(object) = mem_canary_tail;

    with_mutex (&memory_lock) {

        track_allocation(allocation);

        live_allocation_count +=1;
        live_bytes_allocated += size;
    }

    return object;
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
    usize old_size = allocation->size;

    with_mutex (&memory_lock) {

        untrack_allocation(allocation);
    }

    const usize header_size = header_bytes();
    const usize tail_size = sizeof(mem_canary_tail) + tail_offset(new_size);
    
    const usize total_new_size = header_size + tail_size;

    AllocHeaderPtr resized = realloc(allocation, total_new_size);
    if (resized == null) {
        panic("%s:%u failed to resize allocation to %zu bytes", file, line, new_size);
    }

    resized->size = new_size;

    ptr object = (bytePtr)resized + header_size;
    *tail_of(object) = mem_canary_tail;

    with_mutex (&memory_lock) {

        live_bytes_allocated -= old_size;
        live_bytes_allocated += new_size;

        track_allocation(resized);
    }

    return object;
}

void
free_function (ptr x, const char *file, u32 line) {
    
    unused(file);
    unused(line);

    if (x == null) {
        return;
    }

    bool verified = mem_verify(x);
    if (!verified) {
        panic("%s:%u header corruption", file, line);
    }


    AllocHeaderPtr allocation = alloc_header_from_object(x);

    with_mutex (&memory_lock) {

        untrack_allocation(allocation);

        live_allocation_count -= 1;
        live_bytes_allocated -= allocation->size;
    }

    free(allocation);
}

#else

static void
mem_init (void) {
}

void
mem_report (void) {
}

bool
mem_verify (ptr x) {

    unused(x);
    return true;
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
resize_function (ptr x, usize new_size, const char *file, u32 line) {

    if (new_size == 0) {
        free_function(x, file, line);
        return null;
    }

    ptr bytes = realloc(x, new_size);

    if (bytes == null) {
        panic("%s:%u failed to resize allocation to %zu bytes", file, line, new_size);
    }

    return bytes;
}

void
free_function (ptr x, const char *file, u32 line) {

    unused(file);
    unused(line);

    free(x);
}

#endif
