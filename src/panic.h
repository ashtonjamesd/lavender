#ifndef panic_h
#define panic_h

#include "common.h"

#define panic(...) panic_at(__FILE__, __LINE__, __func__, __VA_ARGS__)

#define assert(condition) do { \
    if (!(condition)) panic("assertion failed '%s'", #condition); \
} while (0);

_Noreturn void
panic_at (const char *file, u32 line, const char *func, const char *fmt, ...)
    __attribute__((format(printf, 4, 5)));

#endif
