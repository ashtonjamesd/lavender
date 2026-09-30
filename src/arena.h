#ifndef arena_h
#define arena_h

#include "common.h"

#include <stdarg.h>

typedef struct ArenaBlock ArenaBlock, *ArenaBlockPtr;

struct ArenaBlock {
    ArenaBlockPtr next;
    usize used;
    usize capacity;

    _Alignas(16) byte data[];
};

typedef struct Arena Arena, *ArenaPtr;

struct Arena {
    ArenaBlockPtr blocks;

};

Arena
arena_new (void);

ptr
arena_alloc (ArenaPtr arena, usize size);

void
arena_free (ArenaPtr arena);

#endif
