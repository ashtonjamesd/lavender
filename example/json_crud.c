#include "lavender.h"
#include "vendor/yyjson/yyjson.h"

#include <pthread.h>

#define max_users 100

typedef struct User User;

struct User {
    i64 id;
    char name[64];
    char email[128];
    bool used;

};

// controllers run on many threads at once, so the store needs a lock
static User users[max_users];
static i64 next_id = 1;
static pthread_mutex_t users_lock = PTHREAD_MUTEX_INITIALIZER;

// all json for a request is built in this thread's pool, which is reset on the
// next request. the server copies the response body first, so nothing is freed
static _Thread_local char pool[64 * 1024];
static _Thread_local yyjson_alc pool_alc;

static yyjson_mut_doc *
new_doc (void) {

    yyjson_alc_pool_init(&pool_alc, pool, sizeof(pool));
    return yyjson_mut_doc_new(&pool_alc);
}

static const char *
write_doc (yyjson_mut_doc *doc) {

    return yyjson_mut_write_opts(doc, 0, &pool_alc, null, null);
}

// e.g. error(notFound, "no such user") gives 404 {"error": "no such user"}
static Response
error (Response (*status)(const char *), const char *message) {

    yyjson_mut_doc *doc = new_doc();
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);
    yyjson_mut_obj_add_str(doc, root, "error", message);

    return json(status(write_doc(doc)));
}

// copies the user's fields into the document, so it is safe after unlocking
static yyjson_mut_val *
user_to_json (yyjson_mut_doc *doc, User *user) {

    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    yyjson_mut_obj_add_int(doc, obj, "id", user->id);
    yyjson_mut_obj_add_strcpy(doc, obj, "name", user->name);
    yyjson_mut_obj_add_strcpy(doc, obj, "email", user->email);

    return obj;
}

// reads ?id=, returning 0 if it is missing or not a positive number
static i64
query_id (Request request) {

    const char *text = query("id");

    if (text == null) {
        return 0;
    }

    char *end;
    i64 id = strtoll(text, &end, 10);

    return (*end == '\0' and id > 0) ? id : 0;
}

// must be called with users_lock held
static User *
find_user (i64 id) {

    foreach (i, max_users) {
        if (users[i].used and users[i].id == id) {
            return &users[i];
        }
    }

    return null;
}

// GET /api/user, or /api/user?id=1
approute (get_user) {

    yyjson_mut_doc *doc = new_doc();

    if (query("id") == null) {
        yyjson_mut_val *list = yyjson_mut_arr(doc);
        yyjson_mut_doc_set_root(doc, list);

        pthread_mutex_lock(&users_lock);

        foreach (i, max_users) {
            if (users[i].used) {
                yyjson_mut_arr_add_val(list, user_to_json(doc, &users[i]));
            }
        }

        pthread_mutex_unlock(&users_lock);

        return json(ok(write_doc(doc)));
    }

    i64 id = query_id(request);

    if (id == 0) {
        return error(badRequest, "id must be a positive number");
    }

    pthread_mutex_lock(&users_lock);

    User *user = find_user(id);

    if (user != null) {
        yyjson_mut_doc_set_root(doc, user_to_json(doc, user));
    }

    pthread_mutex_unlock(&users_lock);

    if (user == null) {
        return error(notFound, "no such user");
    }

    return json(ok(write_doc(doc)));
}

// POST /api/user with {"name": "...", "email": "..."}
approute (create_user) {

    yyjson_doc *body = yyjson_read(request.body, request.body_len, 0);
    yyjson_val *root = yyjson_doc_get_root(body);

    if (body == null or !yyjson_is_obj(root)) {
        yyjson_doc_free(body);
        return error(badRequest, "body must be a json object");
    }

    yyjson_val *name = yyjson_obj_get(root, "name");
    yyjson_val *email = yyjson_obj_get(root, "email");

    if (!yyjson_is_str(name) or !yyjson_is_str(email)) {
        yyjson_doc_free(body);
        return error(unprocessableContent, "name and email must be strings");
    }

    pthread_mutex_lock(&users_lock);

    User *user = null;

    foreach (i, max_users) {
        if (!users[i].used) {
            user = &users[i];
            break;
        }
    }

    if (user != null) {
        user->used = true;
        user->id = next_id;
        next_id += 1;

        snprintf(user->name, sizeof(user->name), "%s", yyjson_get_str(name));
        snprintf(user->email, sizeof(user->email), "%s", yyjson_get_str(email));
    }

    yyjson_doc_free(body);

    yyjson_mut_doc *doc = new_doc();

    if (user != null) {
        yyjson_mut_doc_set_root(doc, user_to_json(doc, user));
    }

    pthread_mutex_unlock(&users_lock);

    if (user == null) {
        return error(insufficientStorage, "too many users");
    }

    return json(created(write_doc(doc)));
}

// PATCH /api/user?id=1 with {"name": "..."} and/or {"email": "..."}
approute (update_user) {

    i64 id = query_id(request);

    if (id == 0) {
        return error(badRequest, "missing or invalid ?id=");
    }

    yyjson_doc *body = yyjson_read(request.body, request.body_len, 0);
    yyjson_val *root = yyjson_doc_get_root(body);

    if (body == null or !yyjson_is_obj(root)) {
        yyjson_doc_free(body);
        return error(badRequest, "body must be a json object");
    }

    yyjson_val *name = yyjson_obj_get(root, "name");
    yyjson_val *email = yyjson_obj_get(root, "email");

    if ((name != null and !yyjson_is_str(name)) or (email != null and !yyjson_is_str(email))) {
        yyjson_doc_free(body);
        return error(unprocessableContent, "name and email must be strings");
    }

    pthread_mutex_lock(&users_lock);

    User *user = find_user(id);

    if (user != null and name != null) {
        snprintf(user->name, sizeof(user->name), "%s", yyjson_get_str(name));
    }

    if (user != null and email != null) {
        snprintf(user->email, sizeof(user->email), "%s", yyjson_get_str(email));
    }

    yyjson_doc_free(body);

    yyjson_mut_doc *doc = new_doc();

    if (user != null) {
        yyjson_mut_doc_set_root(doc, user_to_json(doc, user));
    }

    pthread_mutex_unlock(&users_lock);

    if (user == null) {
        return error(notFound, "no such user");
    }

    return json(ok(write_doc(doc)));
}

// DELETE /api/user?id=1
approute (delete_user) {

    i64 id = query_id(request);

    if (id == 0) {
        return error(badRequest, "missing or invalid ?id=");
    }

    pthread_mutex_lock(&users_lock);

    User *user = find_user(id);

    if (user != null) {
        user->used = false;
    }

    pthread_mutex_unlock(&users_lock);

    if (user == null) {
        return error(notFound, "no such user");
    }

    return noContent("");
}

int main(void) {
    App x = app(3000);
    debug(&x, true);

    within (x, "api") {
        resource(x, user);
    }

    run(x);

    return 0;
}
