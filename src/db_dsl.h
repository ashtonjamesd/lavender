#ifndef db_dsl
#define db_dsl

#include "common.h"
#include "db.h"

typedef enum DbColumnFlag DbColumnFlag;

enum DbColumnFlag {
    no_flags = 0,
    not_null = 1 << 0,
    primary_key = 1 << 1,
    unique = 1 << 2,
};

typedef struct DbColumn DbColumn;

struct DbColumn {
    char *name;
    DbType type;
    u32 flags;

};

// a text database column
DbColumn
text (char *name, u32 flags);

// an integer database column
DbColumn
integer (char *name, u32 flags);

#define table(name, ...) \
    create_db_table( \
        name, (DbColumn[]) __VA_ARGS__, sizeof((DbColumn[]) __VA_ARGS__) / sizeof(DbColumn) \
    )

// sqlite3 max column count
#define max_columns 2000

typedef struct Columns Columns;

struct Columns {
    DbColumn list[max_columns];

};

bool
create_db_table (char *name, DbColumn *columns, usize count);

bool
create_table (char *name, Columns columns);

#endif