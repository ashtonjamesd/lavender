#ifndef panic_h
#define panic_h

#include "common.h"

#define panic(...) panic_at(__FILE__, __LINE__, __func__, __VA_ARGS__)

_Noreturn void
panic_at (const char *file, u32 line, const char *func, const char *fmt, ...)
    __attribute__((format(printf, 4, 5)));

#endif
