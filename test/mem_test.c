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

int
main (void) {

    return test_results(CLAIM_VV);
}
