#include "lavender.h"

static _Thread_local char buffer[256];

approute (greet) {

    const char *name = query("name");

    if (name == null) {
        return badRequest("missing ?name=");
    }

    snprintf(buffer, sizeof(buffer), "Hello, %s!", name);
    return ok(buffer);
}

approute (whoami) {

    const char *token = header("Authorization");

    if (token == null) {
        return unauthorized("missing Authorization header");
    }

    snprintf(buffer, sizeof(buffer), "your token is '%s'", token);
    return ok(buffer);
}

approute (echo) {

    if (request.body_len == 0) {
        return badRequest("empty body");
    }

    snprintf(buffer, sizeof(buffer), "received %zu bytes: %s", request.body_len, request.body);
    return created(buffer);
}

approute (teapot) {
    return imATeapot("short and stout");
}

int main() {
    App x = app(3000);
    debug(&x, true);

    get(x, greet);
    get(x, whoami);
    post(x, echo);
    get(x, teapot);

    run(x);

    return 0;
}
