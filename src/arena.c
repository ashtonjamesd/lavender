#include "arena.h"

#include <stdarg.h>

#define arena_block_size (4 * KB)
#define arena_alignment 16

static usize
round_up (usize size) {

    return (size + arena_alignment - 1) / arena_alignment * arena_alignment;
}

Arena
arena_new (void) {

    return (Arena) {
        .blocks = null,
    };
}

ptr
arena_alloc (ArenaPtr arena, usize size) {

    usize rounded = round_up(size);
    ArenaBlockPtr block = arena->blocks;

    if (block == null or block->used + rounded > block->capacity) {
        usize capacity = rounded > arena_block_size ? rounded : arena_block_size;

        block = alloc_bytes(sizeof(ArenaBlock) + capacity);
        block->next = arena->blocks;
        block->used = 0;
        block->capacity = capacity;

        arena->blocks = block;
    }

    ptr memory = block->data + block->used;
    block->used += rounded;

    return memory;
}

void
arena_free (ArenaPtr arena) {

    ArenaBlockPtr block = arena->blocks;

    while (block != null) {
        ArenaBlockPtr next = block->next;
        
        dealloc(block);
        block = next;
    }

    arena->blocks = null;
}
