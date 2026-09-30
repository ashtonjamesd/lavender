#include "lavender.h"

approute (home) {
    return ok("Hello World!");
}

int main(void) {
    App x = app(3000);

    within (x, "api/v1")
        get(x, home);

    run(x);

    return 0;
}