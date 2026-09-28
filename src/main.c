#include "app.h"

approute (home) {
    (void)request;
    return ok("Hello World!");
}

int main() {
    App x = app(3000);

    within (&x, "api/v1") {
        get(&x, home);
    }

    run(&x);

    return 0;
}