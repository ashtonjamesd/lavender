#include "server.h"

#include "str.h"

#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <microhttpd.h>


static ptr
microhttpd_start (const char *host, u16 port, RequestHandler handler, ptr context);

static void
microhttpd_stop (ptr handle);

static const char *
microhttpd_header (ptr connection, const char *name);

static const char *
microhttpd_query (ptr connection, const char *name);

const Server microhttpd_server = {
    .name = "libmicrohttpd",
    .server_start = microhttpd_start,
    .server_stop = microhttpd_stop,
    .server_header = microhttpd_header,
    .server_query = microhttpd_query,
};

typedef struct MicrohttpdServer MicrohttpdServer, *MicrohttpdServerPtr;

struct MicrohttpdServer {
    struct MHD_Daemon *daemon;
    RequestHandler handler;
    ptr context;

};

// internal context, separate from the user api
// used to manage the incoming request body
typedef struct RequestContext RequestContext, *RequestContextPtr;

struct RequestContext {
    string body;
    bool body_too_large;

};

#define max_body_size 8 * MB
#define connection_timeout_seconds 30

static bool
parse_method (const char *method, HttpType *type) {

    if (strcmp(method, "GET") == 0 or strcmp(method, "HEAD") == 0) {
        *type = HttpGet;
    } else if (strcmp(method, "POST") == 0) {
        *type = HttpPost;
    } else if (strcmp(method, "PUT") == 0) {
        *type = HttpPut;
    } else if (strcmp(method, "PATCH") == 0) {
        *type = HttpPatch;
    } else if (strcmp(method, "DELETE") == 0) {
        *type = HttpDelete;
    } else if (strcmp(method, "OPTIONS") == 0) {
        *type = HttpOptions;
    } else {
        return false;
    }

    return true;
}

static enum MHD_Result
send_response (struct MHD_Connection *connection, Response response) {

    const char *body = response.body == null ? "" : response.body;

    struct MHD_Response *mhd_response = 
        MHD_create_response_from_buffer_copy(strlen(body), body);

    if (mhd_response == null) {
        return MHD_NO;
    }

    const char *content_type = response.content_type == null
        ? text_plain "; charset=utf-8"
        : response.content_type;

    MHD_add_response_header(mhd_response, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);

    enum MHD_Result result = MHD_queue_response(connection, response.status, mhd_response);
    MHD_destroy_response(mhd_response);

    return result;
}

// called by libmicrohttpd several times per request, first when the headers
// arrive, once per chunk, and when the body is complete
static enum MHD_Result
handle_request (
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **req_cls
) {

    (void)version;

    MicrohttpdServerPtr server = cls;
    RequestContextPtr context = *req_cls;

    // called for headers only
    if (context == null) {

        // reject an oversized body before reading any of it
        const char *length = MHD_lookup_connection_value(
            connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_LENGTH
        );

        if (length != null and strtoull(length, null, 10) > max_body_size) {
            return send_response(connection, contentTooLarge("request body too large"));
        }

        context = alloc(RequestContext);
        *context = (RequestContext) {
            .body = null_string(),
            .body_too_large = false,
        };

        *req_cls = context;
        return MHD_YES;
    }

    // a chunk of the body has arrived
    if (*upload_data_size != 0) {
        if (context->body.len + *upload_data_size > max_body_size) {
            context->body_too_large = true;
        }

        if (!context->body_too_large) {
            string chunk = make_string(0, *upload_data_size, (bytePtr)upload_data);
            string_append(&context->body, chunk);
        }

        *upload_data_size = 0;
        return MHD_YES;
    }

    if (context->body_too_large) {
        return send_response(connection, contentTooLarge("request body too large"));
    }

    HttpType type;
    if (!parse_method(method, &type)) {
        return send_response(connection, notImplemented("unsupported method"));
    }

    // null-terminate the body so controllers can treat it as a c string
    // we do this because users would otherwise be forced to use our strings API
    usize body_len = context->body.len;
    string_append(&context->body, make_string(0, 1, (bytePtr)""));

    Request request = {
        .method = type,
        .path = url,
        .body = context->body._ptr,
        .body_len = body_len,
        ._server = &microhttpd_server,
        ._connection = connection,
    };

    return send_response(connection, server->handler(server->context, request));
}

// called once a request is finished, whether it succeeded or not
static void
request_completed (
    void *cls,
    struct MHD_Connection *connection,
    void **req_cls,
    enum MHD_RequestTerminationCode toe
) {

    (void)cls;
    (void)connection;
    (void)toe;

    RequestContextPtr context = *req_cls;

    if (context == null) {
        return;
    }

    string_destroy(&context->body);
    dealloc(context);
    *req_cls = null;
}

static ptr
microhttpd_start (const char *host, u16 port, RequestHandler handler, ptr context) {

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };

    if (strcmp(host, "localhost") == 0) {
        host = "127.0.0.1";
    }

    if (inet_pton(AF_INET, host, &address.sin_addr) != 1) {
        return null;
    }

    // writing to a socket the client already closed must not kill the process
    signal(SIGPIPE, SIG_IGN);

    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    unsigned int thread_count = cores > 0 ? (unsigned int)cores : 1;

    MicrohttpdServerPtr server = alloc(MicrohttpdServer);
    *server = (MicrohttpdServer) {
        .daemon = null,
        .handler = handler,
        .context = context,
    };

    server->daemon = MHD_start_daemon(
        MHD_USE_AUTO_INTERNAL_THREAD | MHD_USE_ERROR_LOG,
        port,
        null, null,
        handle_request, server,
        MHD_OPTION_SOCK_ADDR, (struct sockaddr *)&address,
        MHD_OPTION_THREAD_POOL_SIZE, thread_count,
        MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)connection_timeout_seconds,
        MHD_OPTION_NOTIFY_COMPLETED, request_completed, null,
        MHD_OPTION_END
    );

    if (server->daemon == null) {
        dealloc(server);
        return null;
    }

    return server;
}

static void
microhttpd_stop (ptr handle) {

    MicrohttpdServerPtr server = handle;

    MHD_stop_daemon(server->daemon);
    dealloc(server);
}

static const char *
microhttpd_header (ptr connection, const char *name) {

    return MHD_lookup_connection_value(connection, MHD_HEADER_KIND, name);
}

static const char *
microhttpd_query (ptr connection, const char *name) {

    return MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, name);
}
