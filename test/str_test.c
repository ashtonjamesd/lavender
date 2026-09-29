#include "claim.h"
#include "str.h"

suite_name ("string")


static bool
has_text (string s, const char *expected) {

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
    expect(has_text(s, "hello world"));
}

it ("gives an empty string for an empty literal") {

    string s = str("");

    expect_eq(s.len, 0u);
    expect_null(s._ptr);
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
    expect(has_text(s, "hello"));

    buffer[0] = 'j';
    expect(has_text(s, "hello"));

    string_destroy(&s);
}

it ("has a capacity of at least its length") {

    string s = string_create("a string longer than eight bytes");

    expect_eq(s.len, 32u);
    expect(s.capacity >= s.len);

    string_destroy(&s);
}

it ("copies a long string exactly") {

    char buffer[1001];
    memset(buffer, 'z', 1000);
    buffer[1000] = '\0';

    string s = string_create(buffer);

    expect_eq(s.len, 1000u);
    expect(memcmp(s._ptr, buffer, 1000) == 0);

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

it ("is safe on a null string") {

    string s = null_string();
    string_destroy(&s);

    expect_null(s._ptr);
    expect_eq(s.len, 0u);
}


describe ("string_append")

it ("appends to a null string") {

    string s = null_string();
    string_append(&s, str("abc"));

    expect(has_text(s, "abc"));
    expect(s.capacity >= s.len);

    string_destroy(&s);
}

it ("appends several times in order") {

    string s = null_string();
    string_append(&s, str("/api"));
    string_append(&s, str("/v1"));
    string_append(&s, str("/home"));

    expect(has_text(s, "/api/v1/home"));

    string_destroy(&s);
}

it ("appends to a string made by string_create") {

    string s = string_create("ab");
    string_append(&s, str("cd"));

    expect(has_text(s, "abcd"));
    expect(s.capacity >= s.len);

    string_destroy(&s);
}

it ("does nothing when appending an empty string") {

    string s = string_create("abc");
    byte *before = s._ptr;

    string_append(&s, str(""));
    string_append(&s, null_string());

    expect(has_text(s, "abc"));
    expect(s._ptr == before);

    string_destroy(&s);
}

it ("allocates nothing when appending empty to empty") {

    string s = null_string();
    string_append(&s, null_string());

    expect_eq(s.len, 0u);
    expect_null(s._ptr);
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

    expect(has_text(s, "abcabc"));

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


describe ("string_eq")

it ("is true for equal contents in different buffers") {

    string a = string_create("hello");
    string b = str("hello");

    expect(a._ptr != b._ptr);
    expect(string_eq(a, b));

    string_destroy(&a);
}

it ("is false for different lengths") {

    refute(string_eq(str("abc"), str("abcd")));
    refute(string_eq(str("abcd"), str("abc")));
}

it ("is false when only the first or last byte differs") {

    refute(string_eq(str("xbc"), str("abc")));
    refute(string_eq(str("abx"), str("abc")));
}

it ("is case sensitive") {

    refute(string_eq(str("ABC"), str("abc")));
}

it ("compares every byte, including null bytes") {

    string a = make_string(0, 3, "a\0b");
    string b = make_string(0, 3, "a\0c");

    refute(string_eq(a, b));
    expect(string_eq(a, make_string(0, 3, "a\0b")));
}

it ("is true for two empty strings") {

    expect(string_eq(null_string(), str("")));
    expect(string_eq(null_string(), null_string()));
}

it ("is false for empty against non-empty") {

    refute(string_eq(null_string(), str("a")));
    refute(string_eq(str("a"), null_string()));
}


describe ("string_starts_with")

it ("recognises the prefixes used to infer routes") {

    expect(string_starts_with(str("get_user"), str("get")));
    expect(string_starts_with(str("create_user"), str("create")));
    expect(string_starts_with(str("update_user"), str("update")));
    expect(string_starts_with(str("delete_user"), str("delete")));
}

it ("is false for a different prefix") {

    refute(string_starts_with(str("get_user"), str("create")));
}

it ("is false when the prefix is longer than the string") {

    refute(string_starts_with(str("ge"), str("get")));
}

it ("is true when the string equals the prefix") {

    expect(string_starts_with(str("get"), str("get")));
}

it ("is true for an empty prefix") {

    expect(string_starts_with(str("anything"), null_string()));
    expect(string_starts_with(null_string(), null_string()));
}

it ("is false for an empty string with a non-empty prefix") {

    refute(string_starts_with(null_string(), str("a")));
}

int
main (void) {

    return test_results(CLAIM_VVV);
}
