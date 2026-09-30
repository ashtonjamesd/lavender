#include "lavender.h"

static _Thread_local char buffer[256];

approute (home) {
    return ok("todo api, see /api/v1/todo");
}

approute (health) {
    return ok("ok");
}

approute (get_todo) {

    const char *id = query("id");

    if (id == null) {
        return ok("all todos");
    }

    snprintf(buffer, sizeof(buffer), "todo %s", id);
    return ok(buffer);
}

approute (create_todo) {

    if (request.body_len == 0) {
        return badRequest("a todo needs some text");
    }

    snprintf(buffer, sizeof(buffer), "created todo: %s", request.body);
    return created(buffer);
}

approute (update_todo) {

    const char *id = query("id");

    if (id == null) {
        return badRequest("missing ?id=");
    }

    snprintf(buffer, sizeof(buffer), "updated todo %s", id);
    return ok(buffer);
}

approute (delete_todo) {

    if (query("id") == null) {
        return badRequest("missing ?id=");
    }

    return noContent("");
}

approute (todo_options) {
    return ok("GET, POST, PATCH, DELETE, OPTIONS");
}

approute (replace_todo) {
    return ok("replaced every todo");
}

approute (get_stats) {

    snprintf(buffer, sizeof(buffer), "stats for %s", request.path);
    return ok(buffer);
}

approute (login) {

    if (header("Authorization") == null) {
        return unauthorized("missing Authorization header");
    }

    return ok("logged in");
}

int main(void) {
    App x = app(3000);
    debug(&x, true);

    root(x, home);
    route(x, "health", HttpGet, health);

    within (x, "api") {
        within (x, "v1") {
            resource(x, todo);

            route(x, "todo", HttpOptions, todo_options);

            use(x, get_stats);
            put(x, replace_todo);

            within (x, "auth") {
                route(x, "login", HttpPost, login);
            }
        }
    }

    run(x);

    return 0;
}
