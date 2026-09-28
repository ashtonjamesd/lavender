#include "panic.h"

#include <stdarg.h>

#if defined(__APPLE__) or defined(__GLIBC__)
    #include <execinfo.h>
    #define has_backtrace 1
#else
    #define has_backtrace 0
#endif

#define max_backtrace_frames 32

static void
print_backtrace (void) {

#if has_backtrace
    void *frames[max_backtrace_frames];
    int count = backtrace(frames, max_backtrace_frames);

    fprintf(stderr, "\nbacktrace:\n");
    fflush(stderr);

    int skipped = 2;

    if (count > skipped) {
        backtrace_symbols_fd(frames + skipped, count - skipped, fileno(stderr));
    }
#endif
}

_Noreturn void
panic_at (const char *file, u32 line, const char *func, const char *fmt, ...) {

    va_list args;

    fflush(stdout);

    fprintf(stderr, "\npanic: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n    at %s:%u in %s()\n", file, line, func);

    print_backtrace();
    fflush(stderr);

    abort();
}
