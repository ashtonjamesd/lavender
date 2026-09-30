#include "lavender.h"

approute (list_books) {
    return ok("all books");
}

approute (create_book) {
    return created("book added");
}

approute (replace_book) {
    return ok("book replaced");
}

approute (update_book) {
    return ok("book edited");
}

approute (delete_book) {
    return noContent("");
}

approute (get_book) {
    return ok("got book");
}

approute (book_options) {
    return ok("GET, POST, PUT, PATCH, DELETE, OPTIONS");
}

int main(void) {
    App x = app(3000);

    within (x, "books") {
        resource (x, book);

        get      (x, list_books);
        put      (x, replace_book);
        options  (x, book_options);
    }

    run(x);

    return 0;
}
