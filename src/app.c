#include "app.h"
#include "db.h"

#include <signal.h>
#include <time.h>

#define initial_route_capacity 8
#define initial_route_groups_capacity 4

// lets only one controller run at a time unless the app is parallel
static pthread_mutex_t controller_lock = PTHREAD_MUTEX_INITIALIZER;
static bool app_created = false;

App 
app (u16 port) {
        
    if (app_created) {
        panic("an app has already been created");
    }
    app_created = true;
    
    usize routes_initial_size = 
        sizeof(Route) * initial_route_capacity;
    
    usize route_groups_initial_size = 
        sizeof(char *) * initial_route_groups_capacity;

    return (App) {
        .host = "127.0.0.1",
        .port = port,
        .debug = false,
        .parallel = false,
        .server = &microhttpd_server,
        .routes = alloc_bytes(routes_initial_size),
        .routes_capacity = initial_route_capacity,
        .routes_count = 0,
        .route_groups = alloc_bytes(route_groups_initial_size),
        .route_groups_capacity = initial_route_groups_capacity,
        .route_groups_count = 0,
        ._do_not_serve = true,
    };
}

void 
debug (AppPtr app, bool debug) {
    app->debug = debug;
}

void
parallel (AppPtr app, bool parallel) {
    app->parallel = parallel;
}

void
host (AppPtr app, const char *host) {
    app->host = host;
}

void
cleanup_app (AppPtr app) {

    for (u32 i = 0; i < app->routes_count; i += 1) {
        string_destroy(&app->routes[i].path);
    }

    dealloc(app->routes);
    dealloc(app->route_groups);

    db_close();
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

Route *
find_route (AppPtr app, HttpType type, const char *path, bool *path_matched) {

    usize path_len = strlen(path);

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

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    Response response;
    bool path_matched;
    Route *route = find_route(app, request.method, request.path, &path_matched);

    if (route == null and path_matched) {
        response = methodNotAllowed("method not allowed");
    } else if (route == null) {
        response = notFound("not found");
    } else if (app->parallel) {
        response = route->controller(request);
    } else {
        with_mutex (&controller_lock) {

            response = route->controller(request);
        }
    }

    if (app->debug) {
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);

        double ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

        printf("%-7s %s %d %.2fms\n", method_name(request.method), request.path, response.status, ms);
        fflush(stdout);
    }

    return response;
}

void
app_run (AppPtr app) {

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

    if (app->_do_not_serve) {
        cleanup_app(app);
        return;
    }

    sigset_t shutdown_signals;
    sigemptyset(&shutdown_signals);
    sigaddset(&shutdown_signals, SIGINT);
    sigaddset(&shutdown_signals, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &shutdown_signals, null);

    ptr handle = app->server->server_start(app->host, app->port, dispatch, app);

    if (handle == null) {
        panic(
            "%s failed to start on %s:%u, is the port in use or the address invalid?",
            app->server->name, app->host, (unsigned int)app->port
        );
    }

    printf("listening on http://%s:%u using %s\n\n", app->host, (unsigned int)app->port, app->server->name);
    fflush(stdout);

    int received;
    sigwait(&shutdown_signals, &received);

    printf("\nreceived %s, shutting down\n", received == SIGINT ? "SIGINT" : "SIGTERM");

    app->server->server_stop(handle);

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

    // you cannot create a root route inside of a group
    if (str_eq(path, "/") and app->route_groups_count > 0) {
        return;
    }

    string url = null_string();

    for (u32 i = 0; i < app->route_groups_count; i += 1) {
        char *group = app->route_groups[i];

        if (group[0] != '/') {
            string_append(&url, str("/"));
        }

        string_append(&url, str(group));
    }

    string_append(&url, str(path));

    // exact route has already been registered
    for (u32 i = 0; i < app->routes_count; i += 1) {
        Route *existing = &app->routes[i];

        if (existing->type == type and string_eq(existing->path, url)) {
            string_destroy(&url);
            return;
        }
    }

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

void
register_inferred_route (AppPtr app, char *path, Controller controller) {
    
    string str_path = string_create(path + 1);
    // '+ 1' skips the leading '/'

    HttpType type = HttpGet;
    if (string_starts_with(str_path, str("get"))) {
        type = HttpGet;
    } else if (string_starts_with(str_path, str("create"))) {
        type = HttpPost;
    } else if (string_starts_with(str_path, str("update"))) {
        type = HttpPatch;
    } else if (string_starts_with(str_path, str("delete"))) {
        type = HttpDelete;
    }

    string_destroy(&str_path);

    register_route(app, path, type, controller);
}