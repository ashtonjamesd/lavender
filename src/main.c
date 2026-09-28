#include "app.h"

approute (home) {
    (void)request;

    return ok("Hello World!");
}

int main() {
    App x = app(3000);
    debug(&x, true);

    within (&x, "api/v1") {
        get(&x, home); // GET api/v1/home
    }

    run(&x);

    return 0;
}