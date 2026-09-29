#include "response.h"

Response
respond (HttpStatus status, const char *body) {

    return (Response) {
        .status = status,
        .body = body,
    };
}

Response
ok (const char *body) {

    return respond(HttpStatusOk, body);
}

Response
created (const char *body) {

    return respond(HttpStatusCreated, body);
}

Response
accepted (const char *body) {

    return respond(HttpStatusAccepted, body);
}

Response
nonAuthoritativeInformation (const char *body) {

    return respond(HttpStatusNonAuthoritativeInformation, body);
}

Response
noContent (const char *body) {

    return respond(HttpStatusNoContent, body);
}

Response
resetContent (const char *body) {

    return respond(HttpStatusResetContent, body);
}

Response
partialContent (const char *body) {

    return respond(HttpStatusPartialContent, body);
}

Response
multiStatus (const char *body) {

    return respond(HttpStatusMultiStatus, body);
}

Response
alreadyReported (const char *body) {

    return respond(HttpStatusAlreadyReported, body);
}

Response
imUsed (const char *body) {

    return respond(HttpStatusImUsed, body);
}

Response
multipleChoices (const char *body) {

    return respond(HttpStatusMultipleChoices, body);
}

Response
movedPermanently (const char *body) {

    return respond(HttpStatusMovedPermanently, body);
}

Response
found (const char *body) {

    return respond(HttpStatusFound, body);
}

Response
seeOther (const char *body) {

    return respond(HttpStatusSeeOther, body);
}

Response
notModified (const char *body) {

    return respond(HttpStatusNotModified, body);
}

Response
temporaryRedirect (const char *body) {

    return respond(HttpStatusTemporaryRedirect, body);
}

Response
permanentRedirect (const char *body) {

    return respond(HttpStatusPermanentRedirect, body);
}

Response
badRequest (const char *body) {

    return respond(HttpStatusBadRequest, body);
}

Response
unauthorized (const char *body) {

    return respond(HttpStatusUnauthorized, body);
}

Response
paymentRequired (const char *body) {

    return respond(HttpStatusPaymentRequired, body);
}

Response
forbidden (const char *body) {

    return respond(HttpStatusForbidden, body);
}

Response
notFound (const char *body) {

    return respond(HttpStatusNotFound, body);
}

Response
methodNotAllowed (const char *body) {

    return respond(HttpStatusMethodNotAllowed, body);
}

Response
notAcceptable (const char *body) {

    return respond(HttpStatusNotAcceptable, body);
}

Response
proxyAuthenticationRequired (const char *body) {

    return respond(HttpStatusProxyAuthenticationRequired, body);
}

Response
requestTimeout (const char *body) {

    return respond(HttpStatusRequestTimeout, body);
}

Response
conflict (const char *body) {

    return respond(HttpStatusConflict, body);
}

Response
gone (const char *body) {

    return respond(HttpStatusGone, body);
}

Response
lengthRequired (const char *body) {

    return respond(HttpStatusLengthRequired, body);
}

Response
preconditionFailed (const char *body) {

    return respond(HttpStatusPreconditionFailed, body);
}

Response
contentTooLarge (const char *body) {

    return respond(HttpStatusContentTooLarge, body);
}

Response
uriTooLong (const char *body) {

    return respond(HttpStatusUriTooLong, body);
}

Response
unsupportedMediaType (const char *body) {

    return respond(HttpStatusUnsupportedMediaType, body);
}

Response
rangeNotSatisfiable (const char *body) {

    return respond(HttpStatusRangeNotSatisfiable, body);
}

Response
expectationFailed (const char *body) {

    return respond(HttpStatusExpectationFailed, body);
}

Response
imATeapot (const char *body) {

    return respond(HttpStatusImATeapot, body);
}

Response
misdirectedRequest (const char *body) {

    return respond(HttpStatusMisdirectedRequest, body);
}

Response
unprocessableContent (const char *body) {

    return respond(HttpStatusUnprocessableContent, body);
}

Response
locked (const char *body) {

    return respond(HttpStatusLocked, body);
}

Response
failedDependency (const char *body) {

    return respond(HttpStatusFailedDependency, body);
}

Response
tooEarly (const char *body) {

    return respond(HttpStatusTooEarly, body);
}

Response
upgradeRequired (const char *body) {

    return respond(HttpStatusUpgradeRequired, body);
}

Response
preconditionRequired (const char *body) {

    return respond(HttpStatusPreconditionRequired, body);
}

Response
tooManyRequests (const char *body) {

    return respond(HttpStatusTooManyRequests, body);
}

Response
requestHeaderFieldsTooLarge (const char *body) {

    return respond(HttpStatusRequestHeaderFieldsTooLarge, body);
}

Response
unavailableForLegalReasons (const char *body) {

    return respond(HttpStatusUnavailableForLegalReasons, body);
}

Response
internalServerError (const char *body) {

    return respond(HttpStatusInternalServerError, body);
}

Response
notImplemented (const char *body) {

    return respond(HttpStatusNotImplemented, body);
}

Response
badGateway (const char *body) {

    return respond(HttpStatusBadGateway, body);
}

Response
serviceUnavailable (const char *body) {

    return respond(HttpStatusServiceUnavailable, body);
}

Response
gatewayTimeout (const char *body) {

    return respond(HttpStatusGatewayTimeout, body);
}

Response
httpVersionNotSupported (const char *body) {

    return respond(HttpStatusHttpVersionNotSupported, body);
}

Response
variantAlsoNegotiates (const char *body) {

    return respond(HttpStatusVariantAlsoNegotiates, body);
}

Response
insufficientStorage (const char *body) {

    return respond(HttpStatusInsufficientStorage, body);
}

Response
loopDetected (const char *body) {

    return respond(HttpStatusLoopDetected, body);
}

Response
notExtended (const char *body) {

    return respond(HttpStatusNotExtended, body);
}

Response
networkAuthenticationRequired (const char *body) {

    return respond(HttpStatusNetworkAuthenticationRequired, body);
}
