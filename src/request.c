#include "request.h"

#include "server.h"

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
