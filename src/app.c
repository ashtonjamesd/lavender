#include "app.h"

#define initial_route_capacity 8
#define initial_route_groups_capacity 4

App 
app (u16 port) {
    usize routes_initial_size = 
        sizeof(Route) * initial_route_capacity;
    
    usize route_groups_initial_size = 
        sizeof(cString) * initial_route_groups_capacity;

    return (App) {
        .port = port,
        .routes = alloc_bytes(routes_initial_size),
        .routes_capacity = initial_route_capacity,
        .routes_count = 0,
        .route_groups = alloc_bytes(route_groups_initial_size),
        .route_groups_capacity = initial_route_groups_capacity,
        .route_groups_count = 0,
    };
}

void
cleanup_app (AppPtr app) {

    for (u32 i = 0; i < app->routes_count; i += 1) {
        string_destroy(&app->routes[i].path);
    }

    dealloc(app->routes);
    dealloc(app->route_groups);
}

void 
run (AppPtr app) {
    printf("server running on port '%d'\n", app->port);

    printf("\n");
    for (u32 i = 0; i < app->routes_count; i += 1) {
        Route route = app->routes[i];

        printf("Route '%.*s'\n", (int)route.path.len, route.path._ptr);
    }

    cleanup_app(app);
}

void
start_group (AppPtr app, cString name) {

    if (app->route_groups_count == app->route_groups_capacity) {
        app->route_groups_capacity *= 2;
        
        usize new_size = app->route_groups_capacity * sizeof(cString);
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
register_route (AppPtr app, cString path, HttpType type, Controller controller) {

    string url = null_string();

    for (u32 i = 0; i < app->route_groups_count; i += 1) {
        cString group = app->route_groups[i];

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
