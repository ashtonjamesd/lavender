#include "lavender.h"

approute (hello) {
    return ok("Hello World!");
}

approute (get_user) {
    return json(ok("{\"id\": 1, \"name\": \"ash\"}"));
}

approute (create_user) {
    return json(created("{\"id\": 2}"));
}

approute (get_order) {
    if (query("id") == null) {
        return json(badRequest("{\"error\": \"missing ?id=\"}"));
    }

    return json(notFound("{\"error\": \"no such order\"}"));
}

approute (page) {
    return with_content_type(ok("<h1>Hello from Lavender</h1>"), "text/html");
}

approute (delete_user) {
    return noContent("");
}

approute (teapot) {
    return imATeapot("I'm a teapot");
}

int main() {
    App x = app(3000);
    debug(&x, true);

    get(x, hello);
    get(x, page);
    get(x, teapot);

    within (x, "api") {
        use(x, get_user);
        use(x, create_user);
        use(x, get_order);
        use(x, delete_user);
    }

    run(x);

    return 0;
}
