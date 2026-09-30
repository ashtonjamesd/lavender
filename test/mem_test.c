#include "claim.h"
#include "common.h"

suite_name ("memory")


typedef struct Point Point;

struct Point {
    i32 x;
    i32 y;

};


describe ("alloc_bytes")

it ("returns memory that can be written and read") {

    byte *bytes = alloc_bytes(64);
    expect_not_null(bytes);

    for (u32 i = 0; i < 64; i += 1) {
        bytes[i] = (byte)i;
    }

    expect(bytes[0] == 0 and bytes[63] == 63);

    dealloc(bytes);
}

should ("return separate memory for each allocation") {

    byte *a = alloc_bytes(16);
    byte *b = alloc_bytes(16);

    expect(a != b);

    memset(a, 'a', 16);
    memset(b, 'b', 16);

    expect(a[15] == 'a' and b[0] == 'b');

    dealloc(a);
    dealloc(b);
}


describe ("alloc")

it ("allocates enough for the given type") {

    Point *point = alloc(Point);

    point->x = 3;
    point->y = 4;

    expect_eq(point->x, 3);
    expect_eq(point->y, 4);

    dealloc(point);
}


describe ("resize")

it ("keeps the contents when growing") {

    byte *bytes = alloc_bytes(4);
    memcpy(bytes, "abcd", 4);

    bytes = resize(bytes, 1024);
    bytes[1023] = 'z';

    expect(memcmp(bytes, "abcd", 4) == 0);

    dealloc(bytes);
}

it ("keeps the start of the contents when shrinking") {

    byte *bytes = alloc_bytes(8);
    memcpy(bytes, "abcdefgh", 8);

    bytes = resize(bytes, 3);

    expect(memcmp(bytes, "abc", 3) == 0);

    dealloc(bytes);
}

it ("allocates when given null") {

    byte *bytes = resize(null, 8);

    expect_not_null(bytes);
    memset(bytes, 'x', 8);

    dealloc(bytes);
}

it ("frees and returns null for a size of 0") {

    byte *bytes = alloc_bytes(8);
    bytes = resize(bytes, 0);

    expect_null(bytes);
}


describe ("dealloc")

should ("set the pointer to null") {

    byte *bytes = alloc_bytes(8);

    dealloc(bytes);

    expect_null(bytes);
}

it ("is safe to call twice") {

    byte *bytes = alloc_bytes(8);

    dealloc(bytes);
    dealloc(bytes);

    expect_null(bytes);
}

it ("is safe on null") {

    byte *bytes = null;

    dealloc(bytes);

    expect_null(bytes);
}


describe ("alignment")

should ("return 16-byte aligned memory for any size") {

    usize sizes[] = { 1, 3, 8, 15, 16, 17, 100, 4096 };

    for (u32 i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i += 1) {
        byte *bytes = alloc_bytes(sizes[i]);

        expect((uintptr_t)bytes % 16 == 0);

        dealloc(bytes);
    }
}

should ("stay aligned after resizing") {

    byte *bytes = alloc_bytes(5);

    bytes = resize(bytes, 1000);
    expect((uintptr_t)bytes % 16 == 0);

    bytes = resize(bytes, 7);
    expect((uintptr_t)bytes % 16 == 0);

    dealloc(bytes);
}


#ifndef MEM_NO_TRACK_LEAKS

describe ("live counts")

should ("count each allocation and its bytes") {

    usize count = mem_live_count();
    usize bytes = mem_live_bytes();

    byte *a = alloc_bytes(10);
    byte *b = alloc_bytes(20);

    expect_eq(mem_live_count(), count + 2);
    expect_eq(mem_live_bytes(), bytes + 30);

    dealloc(a);
    dealloc(b);
}

should ("return to the starting counts after freeing") {

    usize count = mem_live_count();
    usize bytes = mem_live_bytes();

    byte *a = alloc_bytes(10);
    Point *point = alloc(Point);

    dealloc(a);
    dealloc(point);

    expect_eq(mem_live_count(), count);
    expect_eq(mem_live_bytes(), bytes);
}

should ("change only the bytes when resizing") {

    usize count = mem_live_count();
    usize bytes = mem_live_bytes();

    byte *a = alloc_bytes(10);

    a = resize(a, 50);
    expect_eq(mem_live_count(), count + 1);
    expect_eq(mem_live_bytes(), bytes + 50);

    a = resize(a, 5);
    expect_eq(mem_live_count(), count + 1);
    expect_eq(mem_live_bytes(), bytes + 5);

    dealloc(a);
}

should ("count resize from null as an allocation") {

    usize count = mem_live_count();

    byte *a = resize(null, 8);
    expect_eq(mem_live_count(), count + 1);

    dealloc(a);
    expect_eq(mem_live_count(), count);
}

should ("count resize to 0 as a free") {

    usize count = mem_live_count();

    byte *a = alloc_bytes(8);
    a = resize(a, 0);

    expect_eq(mem_live_count(), count);
}

should ("not change the counts when freeing null") {

    usize count = mem_live_count();
    usize bytes = mem_live_bytes();

    byte *nothing = null;
    dealloc(nothing);

    expect_eq(mem_live_count(), count);
    expect_eq(mem_live_bytes(), bytes);
}

static void *
allocate_and_free (void *n) {

    unused(n);

    for (u32 i = 0; i < 1000; i += 1) {
        byte *a = alloc_bytes(16);
        a = resize(a, 32);
        dealloc(a);
    }

    return null;
}

should ("keep correct counts across threads") {

    usize count = mem_live_count();
    usize bytes = mem_live_bytes();

    pthread_t threads[8];

    for (u32 i = 0; i < 8; i += 1) {
        pthread_create(&threads[i], null, allocate_and_free, null);
    }

    for (u32 i = 0; i < 8; i += 1) {
        pthread_join(threads[i], null);
    }

    expect_eq(mem_live_count(), count);
    expect_eq(mem_live_bytes(), bytes);
}


describe ("guards")

should ("be intact when every byte of the object is used") {

    usize sizes[] = { 1, 5, 8, 13, 16, 100 };

    for (u32 i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i += 1) {
        byte *bytes = alloc_bytes(sizes[i]);
        memset(bytes, 'x', sizes[i]);

        expect(mem_verify(bytes));

        dealloc(bytes);
    }
}

should ("treat null as intact") {

    expect(mem_verify(null));
}

should ("detect a write just before the object") {

    byte *bytes = alloc_bytes(8);
    bytes[-1] = 'x';

    expect_not(mem_verify(bytes));
}

should ("detect a write several bytes before the object") {

    byte *bytes = alloc_bytes(8);
    bytes[-8] = 'x';

    expect_not(mem_verify(bytes));
}


describe ("tails")

should ("detect a write just past an object that fills its last 8 bytes") {

    byte *bytes = alloc_bytes(16);
    bytes[16] = 'x';

    expect_not(mem_verify(bytes));
}

should ("detect a write into the tail of an object that does not fill its last 8 bytes") {

    byte *bytes = alloc_bytes(5);
    bytes[8] = 'x';

    expect_not(mem_verify(bytes));
}

should ("detect a write to the last byte of the tail") {

    byte *bytes = alloc_bytes(16);
    bytes[16 + 7] = 'x';

    expect_not(mem_verify(bytes));
}

should ("move the tail to the new end when growing") {

    byte *bytes = alloc_bytes(8);
    bytes = resize(bytes, 64);
    memset(bytes, 'x', 64);

    expect(mem_verify(bytes));

    bytes[64] = 'x';
    expect_not(mem_verify(bytes));
}

should ("move the tail to the new end when shrinking") {

    byte *bytes = alloc_bytes(64);
    bytes = resize(bytes, 8);
    memset(bytes, 'x', 8);

    expect(mem_verify(bytes));

    bytes[8] = 'x';
    expect_not(mem_verify(bytes));
}

should ("keep the head guard through a resize") {

    byte *bytes = alloc_bytes(8);
    bytes = resize(bytes, 1000);

    expect(mem_verify(bytes));

    bytes[-1] = 'x';
    expect_not(mem_verify(bytes));
}


#endif

int
main (void) {

    return test_results(CLAIM_VV);
}
