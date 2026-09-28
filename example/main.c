#include "app.h"

approute (home) {
    return ok("Hello World!");
}

int main() {
    // create an new application instance, port 3000
    App x = app(3000);

    // group endpoints
    within (&x, "api/v1") {
       
        // register a request endpoint
        get(&x, home);
    }

    // run the web server
    run(&x);

    return 0;
}