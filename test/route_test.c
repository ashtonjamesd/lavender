#include "claim.h"
#include "common.h"

#include "app.h"

suite_name ("router")


describe ("route")

approute(some_get_route){ return ok(""); }
should ("register a get route") {
    App x = app(1234);

    get(x, some_get_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/some_get_route", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_get_route);
    expect(route->type == HttpGet);
    expect(path_matched);
}

approute(some_post_route){ return ok(""); }
should ("register a post route under its group") {
    App x = app(1234);

    within (x, "api") {
        post(x, some_post_route);
    }

    bool path_matched;
    Route *route = find_route(&x, HttpPost, "/api/some_post_route", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_post_route);
    expect(route->type == HttpPost);

    Route *wrong_method = find_route(&x, HttpGet, "/api/some_post_route", &path_matched);

    expect_null(wrong_method);
    expect(path_matched);
}

approute(create_thing){ return ok(""); }
should ("infer the method from the controller name") {
    App x = app(1234);

    use(x, create_thing);

    bool path_matched;
    Route *route = find_route(&x, HttpPost, "/create_thing", &path_matched);

    expect_not_null(route);
    expect(route->type == HttpPost);
}

should ("not find a path that was never registered") {
    App x = app(1234);

    get(x, some_get_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/missing", &path_matched);

    expect_null(route);
    expect_not(path_matched);
}

approute(get_item){ return ok(""); }
approute(create_item){ return ok(""); }
approute(update_item){ return ok(""); }
approute(delete_item){ return ok(""); }
should ("register all four resource routes on one path") {
    App x = app(1234);

    resource(x, item);

    bool path_matched;

    expect(find_route(&x, HttpGet, "/item", &path_matched)->controller == get_item);
    expect(find_route(&x, HttpPost, "/item", &path_matched)->controller == create_item);
    expect(find_route(&x, HttpPatch, "/item", &path_matched)->controller == update_item);
    expect(find_route(&x, HttpDelete, "/item", &path_matched)->controller == delete_item);
}

should ("ignore a trailing slash on the request path") {
    App x = app(1234);

    get(x, some_get_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/some_get_route/", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_get_route);
}

should ("join nested groups into one path") {
    App x = app(1234);

    within (x, "api") {
        within (x, "v1") {
            get(x, some_get_route);
        }
    }

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/api/v1/some_get_route", &path_matched);

    expect_not_null(route);
}

should ("register a custom path and method with route") {
    App x = app(1234);

    route(x, "ping", HttpOptions, some_get_route);

    bool path_matched;
    Route *found = find_route(&x, HttpOptions, "/ping", &path_matched);

    expect_not_null(found);
    expect(found->type == HttpOptions);
    expect(found->controller == some_get_route);
}

should ("register the root path") {
    App x = app(1234);

    root(x, some_get_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_get_route);
}


describe ("method macros")

approute(replace_thing){ return ok(""); }
should ("register a put route") {
    App x = app(1234);

    put(x, replace_thing);

    bool path_matched;
    Route *route = find_route(&x, HttpPut, "/replace_thing", &path_matched);

    expect_not_null(route);
    expect(route->type == HttpPut);
}

approute(edit_thing){ return ok(""); }
should ("register a patch route") {
    App x = app(1234);

    patch(x, edit_thing);

    bool path_matched;
    Route *route = find_route(&x, HttpPatch, "/edit_thing", &path_matched);

    expect_not_null(route);
    expect(route->type == HttpPatch);
}

approute(remove_thing){ return ok(""); }
should ("register a delete route") {
    App x = app(1234);

    delete(x, remove_thing);

    bool path_matched;
    Route *route = find_route(&x, HttpDelete, "/remove_thing", &path_matched);

    expect_not_null(route);
    expect(route->type == HttpDelete);
}

approute(thing_options){ return ok(""); }
should ("register an options route") {
    App x = app(1234);

    options(x, thing_options);

    bool path_matched;
    Route *route = find_route(&x, HttpOptions, "/thing_options", &path_matched);

    expect_not_null(route);
    expect(route->type == HttpOptions);
}

should ("register a route directly with register_route") {
    App x = app(1234);

    register_route(&x, "/direct", HttpPut, some_get_route);

    bool path_matched;
    Route *route = find_route(&x, HttpPut, "/direct", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_get_route);
}


describe ("use")

should ("infer get from a get_ prefix") {
    App x = app(1234);

    use(x, get_item);

    bool path_matched;
    expect_not_null(find_route(&x, HttpGet, "/get_item", &path_matched));
}

approute(update_thing){ return ok(""); }
should ("infer patch from an update_ prefix") {
    App x = app(1234);

    use(x, update_thing);

    bool path_matched;
    expect_not_null(find_route(&x, HttpPatch, "/update_thing", &path_matched));
}

approute(delete_thing){ return ok(""); }
should ("infer delete from a delete_ prefix") {
    App x = app(1234);

    use(x, delete_thing);

    bool path_matched;
    expect_not_null(find_route(&x, HttpDelete, "/delete_thing", &path_matched));
}

approute(list_things){ return ok(""); }
should ("default to get for any other name") {
    App x = app(1234);

    use(x, list_things);

    bool path_matched;
    expect_not_null(find_route(&x, HttpGet, "/list_things", &path_matched));
}


describe ("groups")

should ("not double a leading slash in a group name") {
    App x = app(1234);

    within (x, "/api") {
        get(x, some_get_route);
    }

    bool path_matched;
    expect_not_null(find_route(&x, HttpGet, "/api/some_get_route", &path_matched));
}

should ("stop prefixing once the group ends") {
    App x = app(1234);

    within (x, "api") {
        post(x, some_post_route);
    }

    get(x, some_get_route);

    bool path_matched;
    expect_not_null(find_route(&x, HttpGet, "/some_get_route", &path_matched));
    expect_null(find_route(&x, HttpGet, "/api/some_get_route", &path_matched));
    expect_eq(x.route_groups_count, 0u);
}

should ("nest more groups than the initial capacity") {
    App x = app(1234);
    char *names[6] = { "a", "b", "c", "d", "e", "f" };

    for (u32 i = 0; i < 6; i += 1) {
        start_group(&x, names[i]);
    }

    get(x, some_get_route);

    for (u32 i = 0; i < 6; i += 1) {
        end_group(&x);
    }

    bool path_matched;
    expect_not_null(find_route(&x, HttpGet, "/a/b/c/d/e/f/some_get_route", &path_matched));
}

should ("apply groups to resource") {
    App x = app(1234);

    within (x, "api") {
        resource(x, item);
    }

    bool path_matched;
    Route *route = find_route(&x, HttpPatch, "/api/item", &path_matched);

    expect_not_null(route);
    expect(route->controller == update_item);
}


describe ("storage")

should ("keep every route when registering more than the initial capacity") {
    App x = app(1234);
    char path[32];

    for (u32 i = 0; i < 20; i += 1) {
        snprintf(path, sizeof(path), "/route_%u", i);
        register_route(&x, path, HttpGet, some_get_route);
    }

    expect_eq(x.routes_count, 20u);

    bool path_matched;

    for (u32 i = 0; i < 20; i += 1) {
        snprintf(path, sizeof(path), "/route_%u", i);
        expect_not_null(find_route(&x, HttpGet, path, &path_matched));
    }
}


describe ("matching")

should ("not match a shorter path") {
    App x = app(1234);

    get(x, some_get_route);

    bool path_matched;
    expect_null(find_route(&x, HttpGet, "/some_get", &path_matched));
    expect_not(path_matched);
}

should ("not match a longer path") {
    App x = app(1234);

    get(x, some_get_route);

    bool path_matched;
    expect_null(find_route(&x, HttpGet, "/some_get_route_extra", &path_matched));
    expect_not(path_matched);
}

should ("match paths case sensitively") {
    App x = app(1234);

    get(x, some_get_route);

    bool path_matched;
    expect_null(find_route(&x, HttpGet, "/SOME_GET_ROUTE", &path_matched));
}


describe ("root in a group")

should ("not register a root route inside a group") {
    App x = app(1234);

    within (x, "api") {
        root(x, some_get_route);
    }

    bool path_matched;
    expect_eq(x.routes_count, 0u);
    expect_null(find_route(&x, HttpGet, "/api", &path_matched));
    expect_null(find_route(&x, HttpGet, "/api/", &path_matched));
}

should ("still register a root route outside a group") {
    App x = app(1234);

    within (x, "api") {
        get(x, some_get_route);
    }

    root(x, some_post_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_post_route);
}


describe ("duplicate routes")

should ("register the same path and method only once") {
    App x = app(1234);

    register_route(&x, "/dup", HttpGet, some_get_route);
    register_route(&x, "/dup", HttpGet, some_post_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/dup", &path_matched);

    expect_eq(x.routes_count, 1u);
    expect(route->controller == some_get_route);
}

should ("register the same path with different methods") {
    App x = app(1234);

    register_route(&x, "/dup", HttpGet, some_get_route);
    register_route(&x, "/dup", HttpPost, some_post_route);

    expect_eq(x.routes_count, 2u);
}

should ("register the same path only once inside a group") {
    App x = app(1234);

    within (x, "api") {
        get(x, some_get_route);
        get(x, some_get_route);
    }

    expect_eq(x.routes_count, 1u);
}

should ("register the same name in and outside a group") {
    App x = app(1234);

    get(x, some_get_route);

    within (x, "api") {
        get(x, some_get_route);
    }

    bool path_matched;
    expect_eq(x.routes_count, 2u);
    expect_not_null(find_route(&x, HttpGet, "/some_get_route", &path_matched));
    expect_not_null(find_route(&x, HttpGet, "/api/some_get_route", &path_matched));
}

int
main (void) {

    return test_results(CLAIM_VV);
}
