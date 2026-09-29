#include "lavender.h"

approute (get_user) {
    return ok("Getting User!");
}

approute (create_user) {
    return ok("Creating User!");
}

approute (delete_user) {
    return ok("Deleting User!");
}

approute (update_user) {
    return ok("Updating User!");
}

int main() {
    App x = app(3000);
    debug(&x, true);

    within (x, "api/v1") {
        use (x, get_user);
        use (x, create_user);
        use (x, delete_user);
        use (x, update_user);
    }

    run(x);

    return 0;
}