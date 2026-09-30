#include "lavender.h"

middleware (log_request) {

    printf("request received: %s\n", request.path);
    return next();
}

middleware (require_login) {

    if (header("Authorization") == null) {
        return unauthorized("please log in");
    }

    return next();
}

middleware (require_admin) {

    const char *role = header("X-Role");

    if (role == null or !str_eq(role, "admin")) {
        return forbidden("admins only");
    }

    return next();
}

approute (home) {
    return ok("welcome");
}

approute (get_profile) {
    return ok("your profile");
}

approute (get_posts) {
    return ok("all posts");
}

approute (delete_post) {
    return ok("post deleted");
}

int main(void) {
    App x = app(3000);
    debug(&x, true);

    // logs everything below
    guard(x, log_request);

    get(x, home);

    guarded (x, require_login) {
        get(x, get_profile);
    }

    within (x, "api") {
        guard(x, require_login);

        get(x, get_posts);

        within (x, "admin") {
            guard(x, require_admin);
            delete(x, delete_post);
        }
    }

    run(&x);

    return 0;
}
