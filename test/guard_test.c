#include "claim.h"
#include "common.h"

#include "app.h"

suite_name ("guard")

static u32 order[16];
static u32 order_count = 0;

static void
record (u32 step) {

    order[order_count] = step;
    order_count += 1;
}

approute (secret) {

    record(99);
    return ok("secret");
}

middleware (allow) {

    record(1);
    return next();
}

middleware (also_allow) {

    record(2);
    return next();
}

middleware (deny) {

    record(3);
    return unauthorized("no");
}

static Response
run_route (AppPtr app, const char *path) {

    bool path_matched;
    Route *route = find_route(app, HttpGet, path, &path_matched);

    return route_run(route, (Request) { .method = HttpGet, .path = path });
}


describe ("running guards")

should ("run the controller when there are no guards") {

    App x = app(1234);
    get(x, secret);

    Response response = run_route(&x, "/secret");

    expect_eq(response.body, "secret");
    expect_eq(order_count, 1u);
}

should ("run the controller when every guard allows it") {

    App x = app(1234);

    guard(x, allow);
    guard(x, also_allow);
    get(x, secret);

    Response response = run_route(&x, "/secret");

    expect_eq(response.body, "secret");
    expect_eq(order_count, 3u);
    expect_eq(order[0], 1u);
    expect_eq(order[1], 2u);
    expect_eq(order[2], 99u);
}

should ("stop at the first guard that returns a response") {

    App x = app(1234);

    guard(x, allow);
    guard(x, deny);
    guard(x, also_allow);
    get(x, secret);

    Response response = run_route(&x, "/secret");

    expect(response.status == HttpStatusUnauthorized);
    expect_eq(response.body, "no");

    expect_eq(order_count, 2u);
    expect_eq(order[0], 1u);
    expect_eq(order[1], 3u);
}


describe ("guard scope")

should ("only apply to routes registered after it") {

    App x = app(1234);

    get(x, secret);
    guard(x, deny);

    Response response = run_route(&x, "/secret");

    expect_eq(response.body, "secret");
}

should ("end with its within group") {

    App x = app(1234);

    within (x, "admin") {
        guard(x, deny);
    }

    get(x, secret);

    expect_eq(x.guards_count, 0u);
    expect_eq(run_route(&x, "/secret").body, "secret");
}

should ("apply inside its group") {

    App x = app(1234);

    within (x, "admin") {
        guard(x, deny);
        get(x, secret);
    }

    expect(run_route(&x, "/admin/secret").status == HttpStatusUnauthorized);
}

should ("add up through nested groups and unwind in order") {

    App x = app(1234);

    within (x, "a") {
        guard(x, allow);

        within (x, "b") {
            guard(x, also_allow);
            get(x, secret);

            expect_eq(x.guards_count, 2u);
        }

        expect_eq(x.guards_count, 1u);
    }

    expect_eq(x.guards_count, 0u);

    run_route(&x, "/a/b/secret");

    expect_eq(order_count, 3u);
    expect_eq(order[0], 1u);
    expect_eq(order[1], 2u);
}

should ("apply to every later route when added outside any group") {

    App x = app(1234);

    guard(x, deny);

    within (x, "api") {
        get(x, secret);
    }

    expect(run_route(&x, "/api/secret").status == HttpStatusUnauthorized);
}

should ("keep a route's guards when more are added later") {

    App x = app(1234);

    guard(x, allow);
    get(x, secret);
    guard(x, deny);

    Response response = run_route(&x, "/secret");
    expect_eq(response.body, "secret");
}

should ("grow past its initial capacity") {

    App x = app(1234);

    foreach (i, 10) {
        guard(x, allow);
    }

    get(x, secret);
    run_route(&x, "/secret");

    expect_eq(x.guards_count, 10u);
    expect_eq(order_count, 11u);
}


describe ("guarded blocks")

should ("guard the routes inside without changing their path") {

    App x = app(1234);

    guarded (x, deny) {
        get(x, secret);
    }

    expect(run_route(&x, "/secret").status == HttpStatusUnauthorized);
}

should ("end with the block") {

    App x = app(1234);

    guarded (x, deny) {
        expect_eq(x.guards_count, 1u);
    }

    get(x, secret);

    expect_eq(x.guards_count, 0u);
    expect_eq(x.route_groups_count, 0u);
    expect_eq(run_route(&x, "/secret").body, "secret");
}

should ("keep the path of the group around it") {

    App x = app(1234);

    within (x, "admin") {
        guarded (x, deny) {
            get(x, secret);
        }
    }

    expect(run_route(&x, "/admin/secret").status == HttpStatusUnauthorized);
}

should ("allow a root route") {

    App x = app(1234);

    guarded (x, deny) {
        root(x, secret);
    }

    expect(run_route(&x, "/").status == HttpStatusUnauthorized);
}

int
main (void) {

    return test_results(CLAIM_VV);
}
