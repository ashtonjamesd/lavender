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

int
main (void) {

    return test_results(CLAIM_VV);
}

