#include "str.h"

#include "common.h"
#include "mem.h"


string
str (char *x) {

    usize len = x == null ? 0 : strlen(x);

    if (len == 0) {
        return null_string();
    }

    return make_string(0, len, x);
}

string 
string_create (char *c_str) {

    usize len = c_str == null ? 0 : strlen(c_str);

    if (len == 0) {
        return null_string();
    }

    usize capacity = len;
    char *bytes = alloc_bytes(len);

    string s = make_string(capacity, len, bytes);
    memcpy(s._ptr, c_str, len);

    return s;
}

void 
string_destroy (stringPtr s_ptr) {

    if (s_ptr->capacity == 0) {
        return;
    }

    s_ptr->capacity = 0;
    s_ptr->len = 0;

    dealloc(s_ptr->_ptr);
}

void
string_append (stringPtr s_ptr, string s_ptr2) {

    if (s_ptr2.len == 0) {
        return;
    }

    u32 total_len = s_ptr->len + s_ptr2.len;

    char *bytes = alloc_bytes(total_len);

    if (s_ptr->len != 0) {
        memcpy(bytes, s_ptr->_ptr, s_ptr->len);
    }

    memcpy(bytes + s_ptr->len, s_ptr2._ptr, s_ptr2.len);

    string_destroy(s_ptr);

    *s_ptr = make_string(total_len, total_len, bytes);
}

bool
string_eq (string s1, string s2) {
    if (s1.len != s2.len) {
        return false;
    }

    foreach (i, s1.len) {
        if (s1._ptr[i] != s2._ptr[i]) {
            return false;
        }
    }

    return true;
}

bool 
string_starts_with (string s1, string s2) {
    if (s2.len > s1.len) return false;

    foreach (i, s2.len) {
        if (s1._ptr[i] != s2._ptr[i]) {
            return false;
        }
    }

    return true;
}