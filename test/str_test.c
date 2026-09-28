#include "claim.h"
#include "str.h"

// strings are not null-terminated, so compare by length and bytes
static bool
string_equals (string s, const char *expected) {

    usize expected_len = strlen(expected);

    if (s.len != expected_len) {
        return false;
    }

    return s.len == 0 or memcmp(s._ptr, expected, s.len) == 0;
}


describe ("str")

it ("wraps a literal without copying it") {

    char *literal = "abc";
    string s = str(literal);

    expect_eq(s.len, 3u);
    expect_eq(s.capacity, 0u);
    expect(s._ptr == literal);
}

it ("works on a char * variable, not just literals") {

    char buffer[] = "hello world";
    char *c_str = buffer;

    string s = str(c_str);

    expect_eq(s.len, 11u);
    expect(string_equals(s, "hello world"));
}

it ("gives an empty string for an empty literal") {

    string s = str("");

    expect_eq(s.len, 0u);
    expect_not_null(s._ptr);
}

it ("gives an empty string for null") {

    char *nothing = null;
    string s = str(nothing);

    expect_eq(s.len, 0u);
    expect_null(s._ptr);
}


describe ("null_string")

it ("is empty, borrowed and points nowhere") {

    string s = null_string();

    expect_eq(s.len, 0u);
    expect_eq(s.capacity, 0u);
    expect_null(s._ptr);
}


describe ("string_create")

it ("returns a null string for null") {

    string s = string_create(null);

    expect_eq(s.len, 0u);
    expect_eq(s.capacity, 0u);
    expect_null(s._ptr);
}

it ("returns a null string for an empty string, allocating nothing") {

    string s = string_create("");

    expect_eq(s.len, 0u);
    expect_eq(s.capacity, 0u);
    expect_null(s._ptr);
}

it ("copies the characters rather than pointing at them") {

    char buffer[] = "hello";
    string s = string_create(buffer);

    expect(s._ptr != buffer);
    expect(string_equals(s, "hello"));

    // changing the source must not change the copy
    buffer[0] = 'j';
    expect(string_equals(s, "hello"));

    string_destroy(&s);
}

it ("has a capacity of at least its length") {

    string s = string_create("a string longer than eight bytes");

    expect_eq(s.len, 32u);
    expect(s.capacity >= s.len);

    string_destroy(&s);
}

it ("is owned, so string_destroy frees it") {

    string s = string_create("owned");

    expect(s.capacity != 0);

    string_destroy(&s);
    expect_null(s._ptr);
}


describe ("string_destroy")

it ("resets the string to empty") {

    string s = string_create("abc");
    string_destroy(&s);

    expect_eq(s.len, 0u);
    expect_eq(s.capacity, 0u);
    expect_null(s._ptr);
}

it ("leaves a borrowed string alone") {

    char *literal = "borrowed";
    string s = str(literal);

    string_destroy(&s);

    expect(s._ptr == literal);
    expect_eq(s.len, 8u);
}

it ("is safe to call twice") {

    string s = string_create("twice");

    string_destroy(&s);
    string_destroy(&s);

    expect_null(s._ptr);
}


describe ("string_append")

it ("appends to a null string") {

    string s = null_string();
    string_append(&s, str("abc"));

    expect(string_equals(s, "abc"));
    expect(s.capacity >= s.len);

    string_destroy(&s);
}

it ("appends several times in order") {

    string s = null_string();
    string_append(&s, str("/api"));
    string_append(&s, str("/v1"));
    string_append(&s, str("/home"));

    expect(string_equals(s, "/api/v1/home"));

    string_destroy(&s);
}

it ("appends to a string made by string_create") {

    string s = string_create("ab");
    string_append(&s, str("cd"));

    expect(string_equals(s, "abcd"));
    expect(s.capacity >= s.len);

    string_destroy(&s);
}

it ("appends to a borrowed string without touching the original") {

    char *literal = "ab";
    string s = str(literal);
    string_append(&s, str("cd"));

    expect(string_equals(s, "abcd"));
    expect(s._ptr != literal);
    expect(s.capacity >= s.len);

    string_destroy(&s);
}

it ("does nothing when appending an empty string") {

    string s = string_create("abc");
    string_append(&s, str(""));

    expect(string_equals(s, "abc"));

    string_destroy(&s);
}

it ("leaves the appended string unchanged") {

    char *literal = "tail";
    string tail = str(literal);

    string s = null_string();
    string_append(&s, tail);

    expect(tail._ptr == literal);
    expect_eq(tail.len, 4u);

    string_destroy(&s);
}

it ("can append a string to itself") {

    string s = string_create("abc");
    string_append(&s, s);

    expect(string_equals(s, "abcabc"));

    string_destroy(&s);
}

it ("handles a long run of appends") {

    string s = null_string();

    for (u32 i = 0; i < 1000; i += 1) {
        string_append(&s, str("x"));
    }

    expect_eq(s.len, 1000u);
    expect(s._ptr[0] == 'x' and s._ptr[999] == 'x');

    string_destroy(&s);
}

int
main (void) {

    return test_results(CLAIM_VV);
}
