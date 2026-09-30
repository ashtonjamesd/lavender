#include "db.h"

#include <sqlite3.h>

static sqlite3 *connection = null;

bool
db_open (const char *path) {

    // fullmutex makes the connection safe to share, even with parallel on
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;

    if (sqlite3_open_v2(path, &connection, flags, null) != SQLITE_OK) {
        return false;
    }

    return true;
}

void
db_close (void) {

    if (connection == null) return;

    sqlite3_close(connection);
    connection = null;
}

bool
db_exec (const char *sql) {

    return sqlite3_exec(connection, sql, null, null, null) == SQLITE_OK;
}

Query
db_query_values (const char *sql, i32 count, DbValue *values) {

    sqlite3_stmt *stmt = null;

    if (sqlite3_prepare_v2(connection, sql, -1, &stmt, null) != SQLITE_OK) {
        panic("failed to prepare \"%s\": %s", sql, sqlite3_errmsg(connection));
    }

    i32 expected = sqlite3_bind_parameter_count(stmt);

    if (count != expected) {
        panic("\"%s\" has %d placeholders but was given %d values", sql, expected, count);
    }

    for (i32 i = 0; i < count; i += 1) {
        DbValue value = values[i];

        if (value.type == DbInt) {
            sqlite3_bind_int64(stmt, i + 1, value.integer);
        } else if (value.type == DbText) {
            sqlite3_bind_text(stmt, i + 1, value.text, -1, SQLITE_TRANSIENT);
        } else {
            sqlite3_bind_null(stmt, i + 1);
        }
    }

    return (Query) {
        .stmt = stmt,
    };
}

i32
db_run_values (const char *sql, i32 count, DbValue *values) {

    Query query = db_query_values(sql, count, values);

    while (db_next(query)) {
    }

    if (!db_done(query)) {
        return -1;
    }

    return sqlite3_changes(connection);
}

bool
db_next (Query query) {

    return sqlite3_step(query.stmt) == SQLITE_ROW;
}

i64
db_int (Query query, i32 column) {

    return sqlite3_column_int64(query.stmt, column);
}

const char *
db_str (Query query, i32 column) {

    return (const char *)sqlite3_column_text(query.stmt, column);
}

bool
db_done (Query query) {

    return sqlite3_finalize(query.stmt) == SQLITE_OK;
}

i64
db_last_id (void) {

    return sqlite3_last_insert_rowid(connection);
}

i32
db_changes (void) {

    return sqlite3_changes(connection);
}

const char *
db_error (void) {

    return sqlite3_errmsg(connection);
}
