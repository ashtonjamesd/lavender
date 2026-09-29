#include "lavender.h"

typedef struct User User;
struct User {
    i64 id;
    char name[64];
    char email[128];
    bool used;

};

#define max_users 100
static User users[max_users];
static i64 next_id = 1;

static void
fill_user (JsonObject object, User *user) {

    json_set_int(object, "id", user->id);
    json_set_str(object, "name", user->name);
    json_set_str(object, "email", user->email);
}

static JsonObject
user_json (User *user) {

    JsonObject object = json_new();
    fill_user(object, user);

    return object;
}

// reads ?id=, returning 0 if it is missing or not a positive number
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

static User *
find_user (i64 id) {

    for (u32 i = 0; i < max_users; i += 1) {
        if (users[i].used and users[i].id == id) {
            return &users[i];
        }
    }

    return null;
}

static User *
free_slot (void) {

    for (u32 i = 0; i < max_users; i += 1) {
        if (!users[i].used) {
            return &users[i];
        }
    }

    return null;
}

// GET /api/user?id=1
approute (get_user) {

    i64 id = query_id(request);
    if (id == 0) {
        return json_error(badRequest, "id must be a positive number");
    }

    User *user = find_user(id);
    if (user == null) {
        return json_error(notFound, "no such user");
    }

    return json(ok(json_write(user_json(user))));
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

    User *user = free_slot();

    if (user == null) {
        json_free(body);
        return json_error(insufficientStorage, "too many users");
    }

    user->used = true;
    user->id = next_id;
    next_id += 1;

    snprintf(user->name, sizeof(user->name), "%s", json_str(name));
    snprintf(user->email, sizeof(user->email), "%s", json_str(email));

    json_free(body);

    return json(created(json_write(user_json(user))));
}

// PATCH /api/user?id=1 with {"name": "..."} and/or {"email": "..."}
approute (update_user) {

    i64 id = query_id(request);
    if (id == 0) {
        return json_error(badRequest, "missing or invalid ?id=");
    }

    User *user = find_user(id);
    if (user == null) {
        return json_error(notFound, "no such user");
    }

    Json body = json_read(request);
    if (!body.ok) {
        return json_error(badRequest, "body must be a json object");
    }

    JsonValue *name = json_get(body, "name");
    JsonValue *email = json_get(body, "email");

    if ((name != null and !json_is_str(name)) or (email != null and !json_is_str(email))) {
        json_free(body);
        return json_error(unprocessableContent, "name and email must be strings");
    }

    if (name != null) {
        snprintf(user->name, sizeof(user->name), "%s", json_str(name));
    }

    if (email != null) {
        snprintf(user->email, sizeof(user->email), "%s", json_str(email));
    }

    json_free(body);

    return json(ok(json_write(user_json(user))));
}

// DELETE /api/user?id=1
approute (delete_user) {

    i64 id = query_id(request);
    if (id == 0) {
        return json_error(badRequest, "missing or invalid ?id=");
    }

    User *user = find_user(id);
    if (user == null) {
        return json_error(notFound, "no such user");
    }
    user->used = false;

    return noContent("");
}

int main() {
    App x = app(3000);
    debug(&x, true);

    within (x, "api") {
        resource(x, user);
    }

    run(x);

    return 0;
}
