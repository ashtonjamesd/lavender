#include "claim.h"
#include "common.h"

#include "app.h"

suite_name ("app")


describe ("app initialisation")

should ("create an app with port") {

    App x = app(1234);
    expect(x.port == 1234);
}


should ("create set debug") {

    App x = app(1234);

    debug(&x, true);

    expect(x.debug);
    debug(&x, false);

    refute(x.debug);
}

should ("listen on localhost by default") {

    App x = app(1234);
    expect_eq(x.host, "127.0.0.1");
}

should ("start with debug and parallel off") {

    App x = app(1234);

    refute(x.debug);
    refute(x.parallel);
}

should ("use libmicrohttpd as the default server") {

    App x = app(1234);
    expect(x.server == &microhttpd_server);
}

should ("start with no routes or groups") {

    App x = app(1234);

    expect_eq(x.routes_count, 0u);
    expect_eq(x.route_groups_count, 0u);
    expect_not_null(x.routes);
    expect_not_null(x.route_groups);
}


describe ("app settings")

should ("set the host") {

    App x = app(1234);

    host(&x, "0.0.0.0");

    expect_eq(x.host, "0.0.0.0");
}

should ("set parallel") {

    App x = app(1234);

    parallel(&x, true);
    expect(x.parallel);

    parallel(&x, false);
    refute(x.parallel);
}


describe ("app groups")

should ("push and pop group names in order") {

    App x = app(1234);

    start_group(&x, "api");
    start_group(&x, "v1");

    expect_eq(x.route_groups_count, 2u);
    expect_eq(x.route_groups[0], "api");
    expect_eq(x.route_groups[1], "v1");

    end_group(&x);
    expect_eq(x.route_groups_count, 1u);
    expect_eq(x.route_groups[0], "api");

    end_group(&x);
    expect_eq(x.route_groups_count, 0u);
}

should ("leave no groups open after within") {

    App x = app(1234);

    within (x, "api") {
        within (x, "v1") {
            expect_eq(x.route_groups_count, 2u);
        }

        expect_eq(x.route_groups_count, 1u);
    }

    expect_eq(x.route_groups_count, 0u);
}

should ("grow past its initial group capacity") {

    App x = app(1234);
    u32 capacity = x.route_groups_capacity;

    foreach (i, capacity + 3) {
        start_group(&x, "g");
    }

    expect_eq(x.route_groups_count, capacity + 3);
    expect(x.route_groups_capacity > capacity);
}

int
main (void) {

    return test_results(CLAIM_VV);
}

