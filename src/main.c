#include "lavender.h"

 
static i64
query_id (Request request) {

    const char *text = query("id");

    if (text == null) {
        return 0;
    }

    char *end;
    i64 id = strtoll(text, &end, 10);

    return (*end == '\0' and id > 0) ? id : 0;
}

static bool
user_json (i64 id, JsonObject *object) {

    Query lookup = db_query("select id, name, email from users where id = ?", id);

    bool found = db_next(lookup);

    if (found) {
        *object = json_new();
        json_set_int(*object, "id", db_int(lookup, 0));
        json_set_str(*object, "name", db_str(lookup, 1));
        json_set_str(*object, "email", db_str(lookup, 2));
    }

    db_done(lookup);

    return found;
}

// GET /api/user?id=1
approute (get_user) {

    i64 id = query_id(request);
    if (id == 0) {
        return json_error(badRequest, "id must be a positive number");
    }

    JsonObject object;
    if (!user_json(id, &object)) {
        return json_error(notFound, "no such user");
    }

    return json(ok(json_write(object)));
}

// POST /api/user with {"name": "...", "email": "..."}
approute (create_user) {

    Json body = json_read(request);
    if (!body.ok) {
        return json_error(badRequest, "body must be a json object");
    }

    JsonValue *name = json_get(body, "name");
    JsonValue *email = json_get(body, "email");

    if (!json_is_str(name) or !json_is_str(email)) {
        json_free(body);
        return json_error(unprocessableContent, "name and email must be strings");
    }

    i32 inserted = db_run(
        "insert into users (name, email) values (?, ?)", json_str(name), json_str(email)
    );
    i64 id = db_last_id();

    json_free(body);

    if (inserted != 1) {
        return json_error(internalServerError, "could not create user");
    }

    JsonObject object;
    user_json(id, &object);

    return json(created(json_write(object)));
}

// PATCH /api/user?id=1 with {"name": "..."} and/or {"email": "..."}
approute (update_user) {

    i64 id = query_id(request);
    if (id == 0) {
        return json_error(badRequest, "missing or invalid ?id=");
    }

    Json body = json_read(request);
    if (!body.ok) {
        return json_error(badRequest, "body must be a json object");
    }

    JsonValue *name = json_get(body, "name");
    JsonValue *email = json_get(body, "email");

    if (!json_is_str(name) or !json_is_str(email)) {
        json_free(body);
        return json_error(unprocessableContent, "name and email must be strings");
    }

    db_run(
        "update users set name = coalesce(?, name), email = coalesce(?, email) where id = ?",
        json_str(name), json_str(email), id
    );

    json_free(body);

    JsonObject object;
    if (!user_json(id, &object)) {
        return json_error(notFound, "no such user");
    }

    return json(ok(json_write(object)));
}

// DELETE /api/user?id=1
approute (delete_user) {

    i64 id = query_id(request);
    if (id == 0) {
        return json_error(badRequest, "missing or invalid ?id=");
    }

    if (db_run("delete from users where id = ?", id) == 0) {
        return json_error(notFound, "no such user");
    }

    return noContent("");
}


void
build_database (void) {

    create_table ("users", (Columns) {
        integer ("id", primary_key),
        text    ("name", not_null),
        text    ("email", not_null | unique),
    });
}

int main(void) {
    
    if (!db_open("users.db")) {
        panic("could not open database: %s", db_error());
    }
    build_database();

    App x = app(3000);
    debug(&x, true);

    within (x, "api") {
        resource(x, user);
    }

    x._do_not_serve = true;

    run(x);

    return 0;
}
 