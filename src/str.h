#ifndef str_h
#define str_h

#include "common.h"

typedef struct string string, *stringPtr;

struct string {
    byte *_ptr;
    
    u32 len;
    u32 capacity;
};

#define str(x) make_string(0, strlen(x == null ? "" : x), x)

#define make_string(_capacity, _len, ptr) (string) { \
            .capacity = _capacity, \
            .len = _len, \
            ._ptr = ptr, \
        }

#define null_string() make_string(0, 0, null)

string 
string_create (char *c_str);

void 
string_destroy (stringPtr s_ptr);

void
string_append (stringPtr s_ptr, string s_ptr2);

#endif
