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

should ("register a controller under a custom name with at") {
    App x = app(1234);

    at(x, "things", some_get_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/things", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_get_route);
}

should ("register the root path") {
    App x = app(1234);

    root(x, some_get_route);

    bool path_matched;
    Route *route = find_route(&x, HttpGet, "/", &path_matched);

    expect_not_null(route);
    expect(route->controller == some_get_route);
}

int
main (void) {

    return test_results(CLAIM_VV);
}
