#include "strings.h"

#include "common.h"
#include "mem.h"

string 
string_create (char *c_str) {

    if (c_str == null) {
        return null_string();
    }

    usize len = strlen(c_str);
    byte *bytes = alloc_bytes(len);
    u32 initial_capacity = 8;

    string s = make_string(initial_capacity, len, bytes);
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

    u32 total_len = s_ptr->len + s_ptr2.len;

    s_ptr->_ptr = resize(s_ptr->_ptr, total_len);

    u32 offset = s_ptr->len;
    memcpy(s_ptr->_ptr + offset, s_ptr2._ptr, s_ptr2.len);
    s_ptr->len = total_len;
    s_ptr->capacity = total_len;
}
