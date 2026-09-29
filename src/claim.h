#ifndef claim_h
#define claim_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <iso646.h>

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#define CLAIM_VVV 0     // failure details, plus pending and skipped tests
#define CLAIM_VV 1      // failure details only
#define CLAIM_V 2       // summary line only
#define CLAIM_SILENT 3  // no output, just the exit code

#define CLAIM_LIGHT_RED "\033[91m"
#define CLAIM_DARK_RED "\033[31m"
#define CLAIM_YELLOW "\033[33m"
#define CLAIM_RESET "\033[0m"

#define CLAIM_EQ_DEF(name, T) \
    static bool claim_eq_##name(T a, T b) { \
        return a == b; \
    }

CLAIM_EQ_DEF(int, int)
CLAIM_EQ_DEF(uint, unsigned int)
CLAIM_EQ_DEF(long, long)
CLAIM_EQ_DEF(ulong, unsigned long)
CLAIM_EQ_DEF(llong, long long)
CLAIM_EQ_DEF(ullong, unsigned long long)
CLAIM_EQ_DEF(short, short)
CLAIM_EQ_DEF(ushort, unsigned short)
CLAIM_EQ_DEF(char, char)
CLAIM_EQ_DEF(uchar, unsigned char)
CLAIM_EQ_DEF(float, float)
CLAIM_EQ_DEF(double, double)
CLAIM_EQ_DEF(bool, bool)

static bool claim_eq_str(const char *a, const char *b) {
    if (!a or !b) return a == b;
    return strcmp(a, b) == 0;
}

#define CLAIM_EQ_FN(x) _Generic((x), \
    int: claim_eq_int, \
    unsigned int: claim_eq_uint, \
    long: claim_eq_long, \
    unsigned long: claim_eq_ulong, \
    long long: claim_eq_llong, \
    unsigned long long: claim_eq_ullong, \
    short: claim_eq_short, \
    unsigned short: claim_eq_ushort, \
    char: claim_eq_char, \
    unsigned char: claim_eq_uchar, \
    float: claim_eq_float, \
    double: claim_eq_double, \
    bool: claim_eq_bool, \
    char *: claim_eq_str, \
    const char *: claim_eq_str \
)

#define FORMAT_TEST_VALUE(x) _Generic((x), \
    int: "%d", \
    unsigned int: "%u", \
    long: "%ld", \
    unsigned long: "%lu", \
    long long: "%lld", \
    unsigned long long: "%llu", \
    short: "%d", \
    unsigned short: "%u", \
    char: "%c", \
    unsigned char: "%u", \
    float: "%g", \
    double: "%g", \
    bool: "%d", \
    char *: "\"%s\"", \
    const char *: "\"%s\"" \
)

typedef void (*TestFunc)(void);

typedef struct {
    const char *name;
    const char *group;
    TestFunc fn;
    TestFunc setup;
    TestFunc teardown;
    bool only;
} RegisteredTest;

#define MAX_TESTS 5000

static struct {
    const char *name;

    RegisteredTest registry[MAX_TESTS];
    size_t count;
    bool has_only;

    const char *group;
    TestFunc setup;
    TestFunc teardown;

    // state of the test running in the child process
    FILE *report;
    size_t failed;
    bool pending;
    bool skipped;
} runner;

static void claim_fail(const char *file, int line, const char *fmt, ...) {
    runner.failed += 1;
    fprintf(runner.report, "  %s:%d: ", file, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(runner.report, fmt, args);
    va_end(args);
}

static void claim_skip(const char *msg) {
    runner.skipped = true;
    ftruncate(fileno(runner.report), 0);
    rewind(runner.report);
    fprintf(runner.report, "  %s\n", msg);
}

#define pending() do { \
    runner.pending = true; \
    return; \
} while (0)

#define skip(msg) do { \
    claim_skip(msg); \
    return; \
} while (0)

#define expect(expr) do { \
    if (!(expr)) claim_fail(__FILE__, __LINE__, "expected '%s' to be true\n", #expr); \
} while (0)

#define refute(expr) do { \
    if (expr) claim_fail(__FILE__, __LINE__, "expected '%s' to be false\n", #expr); \
} while (0)

#define expect_not refute

#define expect_null(expr) do { \
    if ((const void *)(expr) != NULL) claim_fail(__FILE__, __LINE__, "expected '%s' to be NULL\n", #expr); \
} while (0)

#define expect_not_null(expr) do { \
    if ((const void *)(expr) == NULL) claim_fail(__FILE__, __LINE__, "expected '%s' to not be NULL\n", #expr); \
} while (0)

#define expect_eq(a, b) do { \
    if (!CLAIM_EQ_FN(a)((a), (b))) { \
        claim_fail(__FILE__, __LINE__, "expected '%s' to equal '%s' (got ", #a, #b); \
        fprintf(runner.report, FORMAT_TEST_VALUE(a), (a)); \
        fprintf(runner.report, ", expected "); \
        fprintf(runner.report, FORMAT_TEST_VALUE(b), (b)); \
        fprintf(runner.report, ")\n"); \
    } \
} while (0)

#define expect_not_eq(a, b) do { \
    if (CLAIM_EQ_FN(a)((a), (b))) { \
        claim_fail(__FILE__, __LINE__, "expected '%s' to not equal '%s' (both are ", #a, #b); \
        fprintf(runner.report, FORMAT_TEST_VALUE(a), (a)); \
        fprintf(runner.report, ")\n"); \
    } \
} while (0)

#define CONCAT2_(a, b) a##b
#define CONCAT_(a, b) CONCAT2_(a, b)

#define _CLAIM_REGISTER(test_name, id, is_only) \
    void CONCAT_(claim_test_, id)(void); \
    __attribute__((constructor)) void CONCAT_(claim_register_, id)(void) { \
        if (runner.count >= MAX_TESTS) { \
            fprintf(stderr, "claim: MAX_TESTS (%d) exceeded\n", MAX_TESTS); \
            exit(1); \
        } \
        runner.registry[runner.count++] = (RegisteredTest){ \
            test_name, runner.group, CONCAT_(claim_test_, id), runner.setup, runner.teardown, is_only \
        }; \
        if (is_only) runner.has_only = true; \
    } \
    void CONCAT_(claim_test_, id)(void)

#define should(name) _CLAIM_REGISTER(name, __COUNTER__, false)
#define it(name) _CLAIM_REGISTER(name, __COUNTER__, false)
#define only(name) _CLAIM_REGISTER(name, __COUNTER__, true)

#define _CLAIM_DESCRIBE(name, id) \
    __attribute__((constructor)) void CONCAT_(claim_describe_, id)(void) { \
        runner.group = name; \
        runner.setup = NULL; \
        runner.teardown = NULL; \
    }

#define describe(name) _CLAIM_DESCRIBE(name, __COUNTER__)

#define _CLAIM_RUNNER_NAME(label, id) \
    __attribute__((constructor)) void CONCAT_(claim_runner_name_, id)(void) { \
        runner.name = label; \
    }

// names this test runner, printed before its results
#define suite_name(label) _CLAIM_RUNNER_NAME(label, __COUNTER__)

#define _CLAIM_HOOK(slot, id) \
    void CONCAT_(claim_hook_, id)(void); \
    __attribute__((constructor)) void CONCAT_(claim_register_hook_, id)(void) { \
        runner.slot = CONCAT_(claim_hook_, id); \
    } \
    void CONCAT_(claim_hook_, id)(void)

#define before(desc) _CLAIM_HOOK(setup, __COUNTER__)
#define after(desc) _CLAIM_HOOK(teardown, __COUNTER__)

static double claim_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

// runs one test in a child process so crashes can't take down the runner
static int claim_run(RegisteredTest *test) {
    ftruncate(fileno(runner.report), 0);
    rewind(runner.report);
    fflush(stdout);

    pid_t pid = fork();
    if (pid < 0) {
        perror("claim: fork");
        exit(1);
    }

    if (pid == 0) {
        if (test->setup) test->setup();
        test->fn();
        if (test->teardown) test->teardown();
        fflush(stdout);

        _exit(runner.pending ? 2 : runner.skipped ? 3 : runner.failed ? 1 : 0);
    }

    int status;
    waitpid(pid, &status, 0);
    return status;
}

static int test_results(int verbosity) {
    size_t passed = 0, failed = 0, pending = 0, skipped = 0;

    runner.report = tmpfile();
    if (!runner.report) {
        perror("claim: tmpfile");
        return 1;
    }
    setvbuf(runner.report, NULL, _IONBF, 0);

    if (runner.name and verbosity < CLAIM_SILENT) {
        printf("\nrunning %s tests\n", runner.name);
    }

    double suite_start = claim_now_ms();
    bool reported = false;

    for (size_t i = 0; i < runner.count; i++) {
        RegisteredTest *test = &runner.registry[i];

        if (runner.has_only and !test->only) {
            skipped += 1;
            continue;
        }

        double test_start = claim_now_ms();
        int status = claim_run(test);
        double test_ms = claim_now_ms() - test_start;
        int code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

        const char *label = "FAIL";
        const char *color = CLAIM_LIGHT_RED;
        if (code == 0) {
            passed += 1;
            continue;
        } else if (code == 2) {
            label = "PENDING";
            color = CLAIM_YELLOW;
            pending += 1;
        } else if (code == 3) {
            label = "SKIP";
            color = CLAIM_YELLOW;
            skipped += 1;
        } else {
            failed += 1;
        }

        bool is_failure = (code != 2 and code != 3);
        if (verbosity > CLAIM_VV or (verbosity == CLAIM_VV and !is_failure)) continue;

        if (WIFSIGNALED(status)) color = CLAIM_DARK_RED;

        reported = true;
        printf("\n%s%s" CLAIM_RESET " ", color, label);
        if (test->group) printf("%s: ", test->group);
        printf("%s (%.1fms)\n", test->name, test_ms);

        rewind(runner.report);
        for (int c; (c = fgetc(runner.report)) != EOF;) putchar(c);

        if (WIFSIGNALED(status)) {
            printf("  " CLAIM_DARK_RED "crashed: %s" CLAIM_RESET "\n", strsignal(WTERMSIG(status)));
        } else if (is_failure and code != 1) {
            printf("  exited with code %d\n", code);
        }
    }

    fclose(runner.report);

    double suite_ms = claim_now_ms() - suite_start;

    if (verbosity < CLAIM_SILENT) {
        // only separate the summary from failure details, not from the runner name
        if (reported) printf("\n");

        printf("%zu tests, %zu passed, %zu failed, %zu pending, %zu skipped in %.1fms\n",
            passed + failed, passed, failed, pending, skipped, suite_ms);
    }

    return failed > 0;
}

#endif