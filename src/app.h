#ifndef app_h
#define app_h

#include "common.h"

#include "str.h"
#include "route.h"
#include "server.h"

typedef struct App App, *AppPtr;

struct App {
    u16 port;
    bool debug;

    // the http server interface
    const Server *server;

    List(Route) routes;
    u32 routes_count;
    u32 routes_capacity;

    List(char *) route_groups;
    u32 route_groups_count;
    u32 route_groups_capacity;
};

// initialises an app structure with a port number
App
app (u16 port);

void 
app_run (AppPtr);

// sets a debug flag for the application
void 
debug (AppPtr, bool debug);

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
#define approute(name) Response name (__attribute__((unused)) Request request)

// runs the application
#define run(x) app_run(&(x))
// defined for consistency with passing around 'x' vs '&'
    
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

#endif