#include "db_dsl.h"

#include "str.h"

static bool
create_db_table (char *name, DbColumn *columns, usize count) {

    string sql = null_string();
    string_append(&sql, str("create table if not exists "));
    string_append(&sql, str(name));
    string_append(&sql, str(" ("));

    for (usize i = 0; i < count; i += 1) {
        DbColumn column = columns[i];

        if (i > 0) {
            string_append(&sql, str(", "));
        }

        string_append(&sql, str(column.name));
        string_append(&sql, str(column.type == DbInt ? " integer" : " text"));

        if (column.flags & primary_key) {
            string_append(&sql, str(" primary key"));
        }

        if (column.flags & not_null) {
            string_append(&sql, str(" not null"));
        }

        if (column.flags & unique) {
            string_append(&sql, str(" unique"));
        }
    }

    string_append(&sql, str(")"));
    string_append(&sql, make_string(0, 1, ""));

    bool created = db_exec(sql._ptr);
    string_destroy(&sql);

    return created;
}

bool
create_table (char *name, Columns columns) {

    usize count = 0;
    while (count < max_columns and columns.list[count].name != null) {
        count += 1;
    }

    return create_db_table(name, columns.list, count);
}

DbColumn
integer (char *name, u32 flags) {

    return (DbColumn) {
        .name = name,
        .type = DbInt,
        .flags = flags,
    };
}

DbColumn
text (char *name, u32 flags) {

    return (DbColumn) {
        .name = name,
        .type = DbText,
        .flags = flags,
    };
}