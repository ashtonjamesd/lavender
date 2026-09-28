#include "app.h"

#include <signal.h>

#define initial_route_capacity 8
#define initial_route_groups_capacity 4

App 
app (u16 port) {
    usize routes_initial_size = 
        sizeof(Route) * initial_route_capacity;
    
    usize route_groups_initial_size = 
        sizeof(char *) * initial_route_groups_capacity;

    return (App) {
        .port = port,
        .debug = false,
        .server = &microhttpd_server,
        .routes = alloc_bytes(routes_initial_size),
        .routes_capacity = initial_route_capacity,
        .routes_count = 0,
        .route_groups = alloc_bytes(route_groups_initial_size),
        .route_groups_capacity = initial_route_groups_capacity,
        .route_groups_count = 0,
    };
}

void 
debug (AppPtr app, bool debug) {
    app->debug = debug;
}

void
cleanup_app (AppPtr app) {

    for (u32 i = 0; i < app->routes_count; i += 1) {
        string_destroy(&app->routes[i].path);
    }

    dealloc(app->routes);
    dealloc(app->route_groups);
}

static const char *
method_name (HttpType type) {

    switch (type) {
        case HttpGet:     return "GET";
        case HttpPost:    return "POST";
        case HttpPatch:   return "PATCH";
        case HttpPut:     return "PUT";
        case HttpDelete:  return "DELETE";
        case HttpOptions: return "OPTIONS";
    }

    return "UNKNOWN";
}

static Route *
find_route (AppPtr app, HttpType type, const char *path, bool *path_matched) {

    usize path_len = strlen(path);

    // treat "/users/" the same as "/users"
    if (path_len > 1 and path[path_len - 1] == '/') {
        path_len -= 1;
    }

    *path_matched = false;

    for (u32 i = 0; i < app->routes_count; i += 1) {
        Route *route = &app->routes[i];

        if (route->path.len != path_len or memcmp(route->path._ptr, path, path_len) != 0) {
            continue;
        }

        *path_matched = true;

        if (route->type == type) {
            return route;
        }
    }

    return null;
}

// the request handler given to the server
static Response
dispatch (ptr context, Request request) {

    AppPtr app = context;

    bool path_matched;
    Route *route = find_route(app, request.method, request.path, &path_matched);

    if (route == null and path_matched) {
        return methodNotAllowed("method not allowed");
    }

    if (route == null) {
        return notFound("not found");
    }

    return route->controller(request);
}

void
run (AppPtr app) {

    if (app->debug) {
        printf("\n");
        for (u32 i = 0; i < app->routes_count; i += 1) {
            Route route = app->routes[i];

            printf("    %-7s %.*s\n", 
                method_name(route.type), (int)route.path.len, route.path._ptr
            );
        }

        printf("\n");
    }

    sigset_t shutdown_signals;
    sigemptyset(&shutdown_signals);
    sigaddset(&shutdown_signals, SIGINT);
    sigaddset(&shutdown_signals, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &shutdown_signals, null);

    ptr handle = app->server->start(app->port, dispatch, app);

    if (handle == null) {
        panic(
            "%s failed to start on port %u, is it already in use?",
            app->server->name, (unsigned int)app->port
        );
    }

    printf("listening on http://localhost:%u using %s\n", (unsigned int)app->port, app->server->name);
    fflush(stdout);

    int received;
    sigwait(&shutdown_signals, &received);

    printf("\nreceived %s, shutting down\n", received == SIGINT ? "SIGINT" : "SIGTERM");

    app->server->stop(handle);
    cleanup_app(app);
}

void
start_group (AppPtr app, char *name) {

    if (app->route_groups_count == app->route_groups_capacity) {
        app->route_groups_capacity *= 2;
        
        usize new_size = app->route_groups_capacity * sizeof(char *);
        app->route_groups = resize(app->route_groups, new_size);
    }

    app->route_groups[app->route_groups_count] = name;
    app->route_groups_count += 1;
}

void 
end_group (AppPtr app) {
    app->route_groups[app->route_groups_count - 1] = null;
    app->route_groups_count -= 1;
}

void
register_route (AppPtr app, char *path, HttpType type, Controller controller) {

    string url = null_string();

    for (u32 i = 0; i < app->route_groups_count; i += 1) {
        char *group = app->route_groups[i];

        if (group[0] != '/') {
            string_append(&url, str("/"));
        }

        string_append(&url, str(group));
    }

    string_append(&url, str(path));

    Route route = {
        .controller = controller,
        .type = type,
        .path = url,
    };

    if (app->routes_count == app->routes_capacity) {
        app->routes_capacity *= 2;

        usize new_size = sizeof(Route) * app->routes_capacity;
        app->routes = resize(app->routes, new_size);
    }

    app->routes[app->routes_count] = route;
    app->routes_count += 1;
}
