#include "lavender.h"

approute (list_books) {
    return ok("all books");
}

approute (add_book) {
    return created("book added");
}

approute (replace_book) {
    return ok("book replaced");
}

approute (edit_book) {
    return ok("book edited");
}

approute (remove_book) {
    return noContent("");
}

approute (book_options) {
    return ok("GET, POST, PUT, PATCH, DELETE, OPTIONS");
}

int main(void) {
    App x = app(3000);
    debug(&x, true);

    within (x, "books") {
        get(x, list_books);
        post(x, add_book);
        put(x, replace_book);
        patch(x, edit_book);
        delete(x, remove_book);
        options(x, book_options);
    }

    run(x);

    return 0;
}
