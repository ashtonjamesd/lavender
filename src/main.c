#include "lavender.h"

middleware (require_key) {
    const char *key = header("X-Api-Key");

    return (!safe_str_eq(key, "KEY"))
        ? unauthorized("no key, no entry")
        : next();
}

approute (hello) {
    return ok("Hello World!");
}

approute (get_secret) {
    return ok("lavender smells nice");
}

int main() {
    App x = app(3000);

    within (x, "api/v1") {
        get(x, hello);

        guarded (x, require_key) {
            use(x, get_secret);
        }
    }

    run(&x);

    return 0;
}
