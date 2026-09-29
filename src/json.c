#include "json.h"

static _Thread_local char pool[64 * 1024];
static _Thread_local yyjson_alc pool_alc;

Json
json_read (Request request) {

    yyjson_doc *body = yyjson_read(request.body, request.body_len, 0);
    yyjson_val *root = yyjson_doc_get_root(body);

    if (body == null or !yyjson_is_obj(root)) {
        yyjson_doc_free(body);

        return (Json) {
            .body = null,
            .root = null,
            .ok = false,
        };
    }

    return (Json) {
        .body = body,
        .root = root,
        .ok = true,
    };
}

void
json_free (Json parsed) {

    yyjson_doc_free(parsed.body);
}

JsonValue *
json_get (Json parsed, const char *key) {

    return yyjson_obj_get(parsed.root, key);
}

bool
json_is_str (JsonValue *value) {

    return yyjson_is_str(value);
}

const char *
json_str (JsonValue *value) {

    return yyjson_get_str(value);
}

JsonObject
json_new (void) {

    yyjson_alc_pool_init(&pool_alc, pool, sizeof(pool));

    yyjson_mut_doc *doc = yyjson_mut_doc_new(&pool_alc);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    return (JsonObject) {
        .doc = doc,
        .root = root,
    };
}

void
json_set_str (JsonObject object, const char *key, const char *value) {

    yyjson_mut_obj_add_strcpy(object.doc, object.root, key, value);
}

void
json_set_int (JsonObject object, const char *key, i64 value) {

    yyjson_mut_obj_add_int(object.doc, object.root, key, value);
}

const char *
json_write (JsonObject object) {

    return yyjson_mut_write_opts(object.doc, 0, &pool_alc, null, null);
}

JsonArray
json_new_array (void) {

    yyjson_alc_pool_init(&pool_alc, pool, sizeof(pool));

    yyjson_mut_doc *doc = yyjson_mut_doc_new(&pool_alc);
    yyjson_mut_val *root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    return (JsonArray) {
        .doc = doc,
        .root = root,
    };
}

JsonObject
json_push_object (JsonArray array) {

    return (JsonObject) {
        .doc = array.doc,
        .root = yyjson_mut_arr_add_obj(array.doc, array.root),
    };
}

const char *
json_write_array (JsonArray array) {

    return yyjson_mut_write_opts(array.doc, 0, &pool_alc, null, null);
}

Response
json_error (Response (*status)(const char *), const char *message) {

    JsonObject object = json_new();
    json_set_str(object, "error", message);

    return json(status(json_write(object)));
}
