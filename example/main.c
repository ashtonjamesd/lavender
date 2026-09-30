#include "lavender.h"

approute (home) {
    return ok("Hello World!");
}

int main(void) {
    App x = app(3000);

    within (x, "my_routes")
        get(x, home);

    run(x);

    return 0;
}