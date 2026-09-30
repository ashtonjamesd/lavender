#ifndef request_h
#define request_h

#include "common.h"
#include "arena.h"

// value of a query string parameter
#define query(name)  request_query(request, name)

// value of a request header
#define header(name) request_header(request, name)

typedef enum HttpType HttpType;

enum HttpType {
    HttpGet,
    HttpPost,
    HttpPatch,
    HttpPut,
    HttpDelete,
    HttpOptions,
};

typedef struct Request Request;

struct Request {
    HttpType method;
    const char *path;
    const char *body;
    usize body_len;

    // required for header and query lookups
    const struct Server *_server;
    ptr _connection;

    // freed after the response is sent
    ArenaPtr _arena;

};

// memory that lives until the response is sent
ptr
request_alloc (Request request, usize size);

// returns the value of a request header, or null if it was not sent
const char *
request_header (Request request, const char *name);

// returns the value of a query string parameter, or null if it was not sent
const char *
request_query (Request request, const char *name);

#endif
