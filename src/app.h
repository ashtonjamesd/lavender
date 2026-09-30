#ifndef app_h
#define app_h

#include "common.h"

#include "str.h"
#include "route.h"
#include "server.h"
#include "concurrency.h"

typedef struct App App, *AppPtr;

struct App {
    const char *host;
    u16 port;
    bool debug;
    bool parallel;

    // true if the web server will not start
    // useful for debugging and tracking memory leaks
    //
    // not part of the user API
    bool _do_not_serve;

    // the http server interface
    const Server *server;

    List(Route) routes;
    u32 routes_count;
    u32 routes_capacity;

    List(char *) route_groups;
    u32 route_groups_count;
    u32 route_groups_capacity;

    // how many guards were active when each group started, to restore when it ends
    List(u32) route_group_guard_counts;

    List(Middleware) guards;
    u32 guards_count;
    u32 guards_capacity;
};

// initialises an app structure with a port number
App
app (u16 port);

// starts the server and blocks until SIGINT or SIGTERM
void
run (AppPtr);

// sets a debug flag for the application, which also logs every request
void
debug (AppPtr, bool debug);

// sets the address to listen on, "127.0.0.1" by default. "0.0.0.0" allows other devices
void
host (AppPtr, const char *host);

// controllers will run without a mutex, and shared data must be managed manually
void
parallel (AppPtr, bool parallel);

void
start_group (AppPtr, char *name);

void 
end_group (AppPtr);

void
register_route (AppPtr, char *path, HttpType, Controller);

void
register_inferred_route (AppPtr, char *path, Controller);

Route *
find_route (AppPtr app, HttpType type, const char *path, bool *path_matched);

// adds a guard to every route registered after it, until the current group ends
void
add_guard (AppPtr, Middleware);

bool
safe_str_eq (const char *given, const char *secret);

// runs a route's guards in order, then its controller if none of them stopped the request
Response
route_run (Route *route, Request request);

// string representation of a HTTP method
const char *
method_name (HttpType type);

// registers a GET route
#define get(app, controller) \
    register_route(&(app), "/" #controller, HttpGet, controller);

// registers a POST route
#define post(app, controller) \
    register_route(&(app), "/" #controller, HttpPost, controller);

// registers a DELETE route
#define delete(app, controller) \
    register_route(&(app), "/" #controller, HttpDelete, controller);

// registers a PUT route
#define put(app, controller) \
    register_route(&(app), "/" #controller, HttpPut, controller);

// registers a PATCH route
#define patch(app, controller) \
    register_route(&(app), "/" #controller, HttpPatch, controller);

// registers an OPTIONS route
#define options(app, controller) \
    register_route(&(app), "/" #controller, HttpOptions, controller);

// registers a route that chooses a HTTP method based on the controller name
#define use(app, controller) \
    register_inferred_route(&(app), "/" #controller, controller);

// defines a HTTP route
#define approute(name) static Response name (__attribute__((unused)) Request request)

// same shape as 'approute'
#define middleware(name) static Response name (__attribute__((unused)) Request request)

#define next_status_ok 0

// lets the request carry on to the next guard or the controller
#define next() ((Response) { .status = next_status_ok })

// guards every route registered after it, until the current within group ends
#define guard(app, check) \
    add_guard(&(app), check)

// registers a controller to handle the root path
#define root(x, controller) \
    register_route(&(x), "/", HttpGet, controller);

#define route(x, path, method, controller) \
    register_route(&(x), "/" path, method, controller);


// registers get_, create_, update_ and delete_ controllers for a resource on one path
#define resource(app, name) \
    register_route(&(app), "/" #name, HttpGet, get_##name); \
    register_route(&(app), "/" #name, HttpPost, create_##name); \
    register_route(&(app), "/" #name, HttpPatch, update_##name); \
    register_route(&(app), "/" #name, HttpDelete, delete_##name);

// starts a group of endpoints that will share a common resource path
#define within(app, group) \
    for ( \
        int _zxqj = (start_group(&(app), group), 1);  \
        _zxqj != 0; \
        end_group(&(app)), _zxqj = 0 \
    )

// guards the routes inside the block, without changing their path
#define guarded(app, check) \
    for ( \
        int _zxqg = (start_group(&(app), null), add_guard(&(app), check), 1);  \
        _zxqg != 0; \
        end_group(&(app)), _zxqg = 0 \
    )

#endif