#include "claim.h"
#include "common.h"

#include "arena.h"
#include "request.h"

suite_name ("arena")


describe ("arena_alloc")

should ("return memory that can be written and read") {

    Arena arena = arena_new();
    byte *bytes = arena_alloc(&arena, 64);

    expect_not_null(bytes);

    foreach (i, 64) {
        bytes[i] = (byte)i;
    }

    expect(bytes[0] == 0 and bytes[63] == 63);

    arena_free(&arena);
}

should ("return 16-byte aligned memory for any size") {

    Arena arena = arena_new();
    usize sizes[] = { 1, 3, 8, 15, 16, 17, 100 };

    foreach (i, sizeof(sizes) / sizeof(sizes[0])) {
        byte *bytes = arena_alloc(&arena, sizes[i]);
        expect((uintptr_t)bytes % 16 == 0);
    }

    arena_free(&arena);
}

should ("not overlap separate allocations") {

    Arena arena = arena_new();

    byte *a = arena_alloc(&arena, 10);
    byte *b = arena_alloc(&arena, 10);

    memset(a, 'a', 10);
    memset(b, 'b', 10);

    expect(a[9] == 'a' and b[0] == 'b');

    arena_free(&arena);
}

should ("keep earlier allocations when it needs a new block") {

    Arena arena = arena_new();
    u32Ptr numbers[1000];

    foreach (i, 1000) {
        numbers[i] = arena_alloc(&arena, sizeof(u32));
        *numbers[i] = i;
    }

    foreach (i, 1000) {
        expect_eq(*numbers[i], i);
    }

    arena_free(&arena);
}

should ("handle an allocation larger than a block") {

    Arena arena = arena_new();
    byte *big = arena_alloc(&arena, 100 * KB);

    memset(big, 'x', 100 * KB);
    expect(big[100 * KB - 1] == 'x');

    arena_free(&arena);
}


describe ("arena_free")

should ("free every block") {

    usize count = mem_live_count();

    Arena arena = arena_new();

    foreach (i, 100) {
        arena_alloc(&arena, KB);
    }

    arena_free(&arena);

    expect_eq(mem_live_count(), count);
    expect_null(arena.blocks);
}

should ("be safe on an empty arena") {

    Arena arena = arena_new();
    arena_free(&arena);

    expect_null(arena.blocks);
}


describe ("request memory")

should ("allocate in the request's arena") {

    Arena arena = arena_new();
    Request request = { ._arena = &arena };

    byte *bytes = request_alloc(request, 32);
    memset(bytes, 'z', 32);

    expect(bytes[31] == 'z');

    arena_free(&arena);
}

int
main (void) {

    return test_results(CLAIM_VV);
}
