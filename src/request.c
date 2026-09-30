#include "request.h"

#include "server.h"

ptr
request_alloc (Request request, usize size) {

    if (request._arena == null) {
        panic("request_alloc needs a request that came from the server");
    }

    return arena_alloc(request._arena, size);
}

const char *
request_header (Request request, const char *name) {

    if (request._server == null) {
        return null;
    }

    return request._server->server_header(request._connection, name);
}

const char *
request_query (Request request, const char *name) {

    if (request._server == null) {
        return null;
    }

    return request._server->server_query(request._connection, name);
}
