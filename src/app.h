#ifndef app_h
#define app_h

#include "common.h"

#include "str.h"
#include "route.h"
#include "server.h"

#define approute(name) Response name (Request request)

#define within(app, group) \
    for (int _zxqj = (start_group(app, group), 1); _zxqj != 0; end_group(app), _zxqj = 0)

#define use

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
run (AppPtr);

void 
debug (AppPtr, bool debug);

void
start_group (AppPtr, char *name);

void 
end_group (AppPtr);

void
register_route (AppPtr, char *path, HttpType type, Controller controller);

#define get(app, controller) \
    register_route(app, "/" #controller, HttpGet, controller);

#define post(app, controller) \
    register_route(app, "/" #controller, HttpPost, controller);

#define delete(app, controller) \
    register_route(app, "/" #controller, HttpDelete, controller);

#define put(app, controller) \
    register_route(app, "/" #controller, HttpPut, controller);

#define patch(app, controller) \
    register_route(app, "/" #controller, HttpPatch, controller);

#endif