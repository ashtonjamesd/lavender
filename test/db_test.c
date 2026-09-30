#include "claim.h"
#include "common.h"

#include "db.h"

suite_name ("database")

static void
open_users (void) {

    db_open(":memory:");
    db_exec("create table users (id integer primary key, name text not null, email text)");
}


describe ("db_open")

should ("open an in-memory database") {

    expect(db_open(":memory:"));
    db_close();
}


describe ("db_exec")

should ("run several statements") {

    open_users();

    expect(db_exec("insert into users (name) values ('a'); insert into users (name) values ('b');"));

    db_close();
}

should ("fail on invalid sql") {

    db_open(":memory:");

    expect_not(db_exec("not sql at all"));
    expect_not_null(db_error());

    db_close();
}


describe ("db_run")

should ("return the rows changed") {

    open_users();

    expect_eq(db_run("insert into users (name, email) values (?, ?)", "ash", "a@x.io"), 1);
    expect_eq(db_run("insert into users (name, email) values (?, ?)", "sam", "s@x.io"), 1);
    expect_eq(db_run("update users set email = ?", "all@x.io"), 2);

    db_close();
}

should ("return 0 when nothing matches") {

    open_users();

    expect_eq(db_run("delete from users where id = ?", 42), 0);

    db_close();
}

should ("return -1 when the statement fails") {

    open_users();

    expect_eq(db_run("insert into users (name) values (?)", null), -1);

    db_close();
}

should ("run sql without any values") {

    open_users();

    expect_eq(db_run("insert into users (name) values ('ash')"), 1);

    db_close();
}

should ("bind a null string as NULL") {

    open_users();

    const char *email = null;
    db_run("insert into users (name, email) values (?, ?)", "ash", email);

    Query query = db_query("select email from users");
    db_next(query);

    expect_null(db_str(query, 0));

    db_done(query);
    db_close();
}


should ("bind up to 16 values") {

    db_open(":memory:");
    db_exec("create table wide (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)");

    i32 changed = db_run(
        "insert into wide values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
        1, "two", 3, 4, 5, 6, 7, 8, "nine", 10, 11, 12, 13, 14, null, 16
    );

    expect_eq(changed, 1);

    Query query = db_query("select a, b, i, o, p from wide");
    db_next(query);

    expect(db_int(query, 0) == 1);
    expect_eq(db_str(query, 1), "two");
    expect_eq(db_str(query, 2), "nine");
    expect_null(db_str(query, 3));
    expect(db_int(query, 4) == 16);

    db_done(query);
    db_close();
}


describe ("db_query")

should ("bind mixed values and read a row") {

    open_users();

    db_run("insert into users (name, email) values (?, ?)", "ash", "a@x.io");
    i64 id = db_last_id();

    Query query = db_query("select id, name, email from users where id = ? and name = ?", id, "ash");

    expect(db_next(query));
    expect(db_int(query, 0) == id);
    expect_eq(db_str(query, 1), "ash");
    expect_eq(db_str(query, 2), "a@x.io");
    expect_not(db_next(query));
    expect(db_done(query));

    db_close();
}

should ("return every row in order") {

    open_users();

    db_exec("insert into users (name) values ('a'), ('b'), ('c')");

    Query query = db_query("select name from users order by id");
    u32 count = 0;

    while (db_next(query)) {
        count += 1;
    }

    expect_eq(count, 3u);

    db_done(query);
    db_close();
}

int
main (void) {

    return test_results(CLAIM_VV);
}
