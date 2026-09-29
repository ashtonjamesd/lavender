#include "claim.h"
#include "common.h"

#include "response.h"

suite_name ("response")


describe ("respond")

should ("default to no content type") {
    Response response = ok("hello");

    expect_null(response.content_type);
    expect(response.status == HttpStatusOk);
    expect_eq(response.body, "hello");
}


describe ("json")

should ("set the json content type") {
    Response response = json(ok("{\"id\": 1}"));

    expect_eq(response.content_type, application_json);
}

should ("keep the status and body") {
    Response response = json(created("{\"id\": 2}"));

    expect(response.status == HttpStatusCreated);
    expect_eq(response.body, "{\"id\": 2}");
}

should ("work with error statuses") {
    Response response = json(notFound("{\"error\": \"missing\"}"));

    expect(response.status == HttpStatusNotFound);
    expect_eq(response.content_type, application_json);
}


describe ("with_content_type")

should ("set any content type") {
    Response response = with_content_type(ok("<h1>hi</h1>"), "text/html");

    expect_eq(response.content_type, "text/html");
}

int
main (void) {

    return test_results(CLAIM_VV);
}
