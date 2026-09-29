#ifndef json_h
#define json_h

#include "common.h"
#include "request.h"
#include "response.h"

#include "vendor/yyjson/yyjson.h"

typedef yyjson_val JsonValue;

typedef struct Json Json;

struct Json {
    yyjson_doc *body;
    yyjson_val *root;
    bool ok;

};

typedef struct JsonObject JsonObject;

struct JsonObject {
    yyjson_mut_doc *doc;
    yyjson_mut_val *root;

};

typedef struct JsonArray JsonArray;

struct JsonArray {
    yyjson_mut_doc *doc;
    yyjson_mut_val *root;

};

// parses the request body, ok is only true if it is a json object
Json
json_read (Request request);

void
json_free (Json parsed);

// a value from the body, or null if the key is missing
JsonValue *
json_get (Json parsed, const char *key);

bool
json_is_str (JsonValue *value);

// the value as a string, or null if it is not one
const char *
json_str (JsonValue *value);

// starts a json object for a response, creating another resets the last one
JsonObject
json_new (void);

// copies the value into the object
void
json_set_str (JsonObject object, const char *key, const char *value);

void
json_set_int (JsonObject object, const char *key, i64 value);

// the object as text, valid until the next json_new on this thread
const char *
json_write (JsonObject object);

// starts a json array for a response, creating another resets the last one
JsonArray
json_new_array (void);

// adds an empty object to the end of the array, fill it with json_set_*
JsonObject
json_push_object (JsonArray array);

// the array as text, valid until the next json_new on this thread
const char *
json_write_array (JsonArray array);

// e.g. json_error(notFound, "no such user") gives 404 {"error": "no such user"}
Response
json_error (Response (*status)(const char *), const char *message);

#endif
