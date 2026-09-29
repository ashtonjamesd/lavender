#ifndef server_h
#define server_h

#include "common.h"
#include "request.h"
#include "response.h"

#define MB 1024 * 1024

typedef Response (*RequestHandler)(ptr context, Request request);

typedef struct Server Server, *ServerPtr;

// a http server backend. the app only talks to the server through these
// functions, so a backend can be easily swapped out.
struct Server {
    const char *name;

    // starts serving on an ipv4 address and port
    ptr (*server_start)(const char *host, u16 port, RequestHandler handler, ptr context);

    // stops accepting new connections
    void (*server_stop)(ptr handle);

    // return a header or query parameter value from a request connection, or null
    const char *(*server_header)(ptr connection, const char *name);
    const char *(*server_query)(ptr connection, const char *name);

};

/* available server backends */

// libmicrohttpd
extern const Server microhttpd_server;

#endif
