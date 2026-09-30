#include "app.h"
#include "db.h"

#include <signal.h>
#include <time.h>

#define initial_route_capacity 8
#define initial_route_groups_capacity 4
#define initial_guards_capacity 4

// lets only one controller run at a time unless the app is parallel
static pthread_mutex_t controller_lock = PTHREAD_MUTEX_INITIALIZER;
static bool app_created = false;

App 
app (u16 port) {
        
    if (app_created) {
        panic("an app has already been created");
    }
    app_created = true;
    
    usize routes_initial_size 
        = sizeof(Route) * initial_route_capacity;
    
    usize route_groups_initial_size 
        = sizeof(char *) * initial_route_groups_capacity;

    usize route_group_guard_counts_size 
        = sizeof(u32) * initial_route_groups_capacity;
    
    usize guards_initial_size 
        = sizeof(Middleware) * initial_guards_capacity;

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
        .route_group_guard_counts = alloc_bytes(route_group_guard_counts_size),
        .guards = alloc_bytes(guards_initial_size),
        .guards_count = 0,
        .guards_capacity = initial_guards_capacity,
        ._do_not_serve = false,
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

static void
cleanup_app (AppPtr app) {

    foreach (i, app->routes_count) {
        string_destroy(&app->routes[i].path);
        dealloc(app->routes[i].guards);
    }

    dealloc(app->routes);
    dealloc(app->route_groups);
    dealloc(app->route_group_guard_counts);
    dealloc(app->guards);

    db_close();
}

const char *
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

    foreach (i, app->routes_count) {
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
        response = route_run(route, request);
    } else {
        with_mutex (&controller_lock) {

            response = route_run(route, request);
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
run (AppPtr app) {

    if (app->route_groups_count != 0) {
        panic("a within or guarded block was left early, so it never ended");
    }

    if (app->debug) {
        printf("\n");
        foreach (i, app->routes_count) {
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

        usize new_counts_size = app->route_groups_capacity * sizeof(u32);
        app->route_group_guard_counts = resize(app->route_group_guard_counts, new_counts_size);
    }

    app->route_groups[app->route_groups_count] = name;
    app->route_group_guard_counts[app->route_groups_count] = app->guards_count;
    app->route_groups_count += 1;
}

void 
end_group (AppPtr app) {
    app->route_groups[app->route_groups_count - 1] = null;
    app->route_groups_count -= 1;

    app->guards_count = app->route_group_guard_counts[app->route_groups_count];
}

void
add_guard (AppPtr app, Middleware check) {

    if (app->guards_count == app->guards_capacity) {
        app->guards_capacity *= 2;

        usize new_size = sizeof(Middleware) * app->guards_capacity;
        app->guards = resize(app->guards, new_size);
    }

    app->guards[app->guards_count] = check;
    app->guards_count += 1;
}

Response
route_run (Route *route, Request request) {

    foreach (i, route->guards_count) {
        Response response = route->guards[i](request);

        // anything other than next() stops the request here
        if (response.status != next_status_ok) {
            return response;
        }
    }

    return route->controller(request);
}

void
register_route (AppPtr app, char *path, HttpType type, Controller controller) {

    string url = null_string();
    bool in_path_group = false;

    foreach (i, app->route_groups_count) {
        char *group = app->route_groups[i];

        // guarded blocks have no path
        if (group == null) {
            continue;
        }

        in_path_group = true;

        if (group[0] != '/') {
            string_append(&url, str("/"));
        }

        string_append(&url, str(group));
    }

    // you cannot create a root route inside of a group
    if (str_eq(path, "/") and in_path_group) {
        string_destroy(&url);
        return;
    }

    string_append(&url, str(path));

    // exact route has already been registered
    foreach (i, app->routes_count) {
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
        .guards = null,
        .guards_count = app->guards_count,
    };

    if (app->guards_count > 0) {
        usize guards_size = sizeof(Middleware) * app->guards_count;

        route.guards = alloc_bytes(guards_size);
        memcpy(route.guards, app->guards, guards_size);
    }

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

bool
safe_str_eq (const char *given, const char *secret) {
    if (given == null or secret == null) {
        return false;
    }

    usize given_len = strlen(given);
    usize secret_len = strlen(secret);

    u8 result = given_len != secret_len;

    for (usize i = 0; i < secret_len; i += 1) {
        u8 given_byte = i < given_len ? (u8)given[i] : 0;
        result |= given_byte ^ (u8)secret[i];
    }

    return result == 0;
}