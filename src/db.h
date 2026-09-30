#ifndef db_h
#define db_h

#include "common.h"

// a prepared sql statement
typedef struct Query Query;

struct Query {
    struct sqlite3_stmt *stmt;

};

// a value for a ? placeholder
typedef enum DbType DbType;

enum DbType {
    DbNull,
    DbInt,
    DbText,
};

typedef struct DbValue DbValue;

struct DbValue {
    DbType type;
    i64 integer;
    const char *text;

};

// opens or creates a sqlite3 database
bool
db_open (const char *path);

// closes a sqlite3 database connection
void
db_close (void);

// runs sql that returns no rows, and may contain several statements
bool
db_exec (const char *sql);

// runs sql with values for its ? placeholders, returns the rows changed or -1 on error
#define db_run(sql, ...) \
    db_run_values(sql, db_args(__VA_ARGS__))

// prepares sql with values for its ? placeholders, read it with db_next then db_done
#define db_query(sql, ...) \
    db_query_values(sql, db_args(__VA_ARGS__))

// moves to the next row, false when there are no more rows or on an error
bool
db_next (Query query);

// a column of the current row
i64
db_int (Query query, i32 column);

// valid until the next db_next or db_done
const char *
db_str (Query query, i32 column);

// frees the query, false if running it failed
bool
db_done (Query query);

// the id of the last inserted row
i64
db_last_id (void);

// the number of rows changed by the last insert, update or delete
i32
db_changes (void);

// the message for the most recent error
const char *
db_error (void);

// internals behind db_run and db_query

i32
db_run_values (const char *sql, i32 count, DbValue *values);

Query
db_query_values (const char *sql, i32 count, DbValue *values);

static inline DbValue
db_value_int (i64 value) {
    return (DbValue) { .type = DbInt, .integer = value };
}

// a null string binds sql NULL
static inline DbValue
db_value_text (const char *value) {
    return (DbValue) { .type = value == null ? DbNull : DbText, .text = value };
}

static inline DbValue
db_value_null (void *value) {
    (void)value;
    return (DbValue) { .type = DbNull };
}

// strings bind as text, null as NULL, and whole numbers as integers
#define db_value(x) _Generic((x), \
        char *: db_value_text, \
        const char *: db_value_text, \
        void *: db_value_null, \
        default: db_value_int \
    )(x)

// counts up to 16 values, and applies db_value to each
#define db_count(...) db_count_(__VA_ARGS__ __VA_OPT__(,) \
    16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define db_count_(_1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16, n, ...) n

#define db_cat(a, b) db_cat_(a, b)
#define db_cat_(a, b) a##b

#define db_map(...) db_cat(db_map_, db_count(__VA_ARGS__))(__VA_ARGS__)
#define db_map_0()
#define db_map_1(a) db_value(a)
#define db_map_2(a, ...) db_value(a), db_map_1(__VA_ARGS__)
#define db_map_3(a, ...) db_value(a), db_map_2(__VA_ARGS__)
#define db_map_4(a, ...) db_value(a), db_map_3(__VA_ARGS__)
#define db_map_5(a, ...) db_value(a), db_map_4(__VA_ARGS__)
#define db_map_6(a, ...) db_value(a), db_map_5(__VA_ARGS__)
#define db_map_7(a, ...) db_value(a), db_map_6(__VA_ARGS__)
#define db_map_8(a, ...) db_value(a), db_map_7(__VA_ARGS__)
#define db_map_9(a, ...) db_value(a), db_map_8(__VA_ARGS__)
#define db_map_10(a, ...) db_value(a), db_map_9(__VA_ARGS__)
#define db_map_11(a, ...) db_value(a), db_map_10(__VA_ARGS__)
#define db_map_12(a, ...) db_value(a), db_map_11(__VA_ARGS__)
#define db_map_13(a, ...) db_value(a), db_map_12(__VA_ARGS__)
#define db_map_14(a, ...) db_value(a), db_map_13(__VA_ARGS__)
#define db_map_15(a, ...) db_value(a), db_map_14(__VA_ARGS__)
#define db_map_16(a, ...) db_value(a), db_map_15(__VA_ARGS__)

// the count and values, with a trailing unused value so the array is never empty
#define db_args(...) \
    db_count(__VA_ARGS__), (DbValue[]){ db_map(__VA_ARGS__) __VA_OPT__(,) db_value_null(null) }

#endif
