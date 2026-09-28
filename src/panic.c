#include "panic.h"

#include <stdarg.h>

_Noreturn void
panic_at (const char *file, u32 line, const char *func, const char *fmt, ...) {

    va_list args;

    fflush(stdout);

    fprintf(stderr, "\npanic: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n    at %s:%u in %s()\n", file, line, func);
    fflush(stderr);

    abort();
}
