#include "response.h"

Response
respond (HttpStatus status, char *body) {

    return (Response) {
        .status = status,
        .body = body,
    };
}

Response
ok (char *body) {

    return respond(HttpStatusOk, body);
}

Response
created (char *body) {

    return respond(HttpStatusCreated, body);
}

Response
accepted (char *body) {

    return respond(HttpStatusAccepted, body);
}

Response
nonAuthoritativeInformation (char *body) {

    return respond(HttpStatusNonAuthoritativeInformation, body);
}

Response
noContent (char *body) {

    return respond(HttpStatusNoContent, body);
}

Response
resetContent (char *body) {

    return respond(HttpStatusResetContent, body);
}

Response
partialContent (char *body) {

    return respond(HttpStatusPartialContent, body);
}

Response
multiStatus (char *body) {

    return respond(HttpStatusMultiStatus, body);
}

Response
alreadyReported (char *body) {

    return respond(HttpStatusAlreadyReported, body);
}

Response
imUsed (char *body) {

    return respond(HttpStatusImUsed, body);
}

Response
multipleChoices (char *body) {

    return respond(HttpStatusMultipleChoices, body);
}

Response
movedPermanently (char *body) {

    return respond(HttpStatusMovedPermanently, body);
}

Response
found (char *body) {

    return respond(HttpStatusFound, body);
}

Response
seeOther (char *body) {

    return respond(HttpStatusSeeOther, body);
}

Response
notModified (char *body) {

    return respond(HttpStatusNotModified, body);
}

Response
temporaryRedirect (char *body) {

    return respond(HttpStatusTemporaryRedirect, body);
}

Response
permanentRedirect (char *body) {

    return respond(HttpStatusPermanentRedirect, body);
}

Response
badRequest (char *body) {

    return respond(HttpStatusBadRequest, body);
}

Response
unauthorized (char *body) {

    return respond(HttpStatusUnauthorized, body);
}

Response
paymentRequired (char *body) {

    return respond(HttpStatusPaymentRequired, body);
}

Response
forbidden (char *body) {

    return respond(HttpStatusForbidden, body);
}

Response
notFound (char *body) {

    return respond(HttpStatusNotFound, body);
}

Response
methodNotAllowed (char *body) {

    return respond(HttpStatusMethodNotAllowed, body);
}

Response
notAcceptable (char *body) {

    return respond(HttpStatusNotAcceptable, body);
}

Response
proxyAuthenticationRequired (char *body) {

    return respond(HttpStatusProxyAuthenticationRequired, body);
}

Response
requestTimeout (char *body) {

    return respond(HttpStatusRequestTimeout, body);
}

Response
conflict (char *body) {

    return respond(HttpStatusConflict, body);
}

Response
gone (char *body) {

    return respond(HttpStatusGone, body);
}

Response
lengthRequired (char *body) {

    return respond(HttpStatusLengthRequired, body);
}

Response
preconditionFailed (char *body) {

    return respond(HttpStatusPreconditionFailed, body);
}

Response
contentTooLarge (char *body) {

    return respond(HttpStatusContentTooLarge, body);
}

Response
uriTooLong (char *body) {

    return respond(HttpStatusUriTooLong, body);
}

Response
unsupportedMediaType (char *body) {

    return respond(HttpStatusUnsupportedMediaType, body);
}

Response
rangeNotSatisfiable (char *body) {

    return respond(HttpStatusRangeNotSatisfiable, body);
}

Response
expectationFailed (char *body) {

    return respond(HttpStatusExpectationFailed, body);
}

Response
imATeapot (char *body) {

    return respond(HttpStatusImATeapot, body);
}

Response
misdirectedRequest (char *body) {

    return respond(HttpStatusMisdirectedRequest, body);
}

Response
unprocessableContent (char *body) {

    return respond(HttpStatusUnprocessableContent, body);
}

Response
locked (char *body) {

    return respond(HttpStatusLocked, body);
}

Response
failedDependency (char *body) {

    return respond(HttpStatusFailedDependency, body);
}

Response
tooEarly (char *body) {

    return respond(HttpStatusTooEarly, body);
}

Response
upgradeRequired (char *body) {

    return respond(HttpStatusUpgradeRequired, body);
}

Response
preconditionRequired (char *body) {

    return respond(HttpStatusPreconditionRequired, body);
}

Response
tooManyRequests (char *body) {

    return respond(HttpStatusTooManyRequests, body);
}

Response
requestHeaderFieldsTooLarge (char *body) {

    return respond(HttpStatusRequestHeaderFieldsTooLarge, body);
}

Response
unavailableForLegalReasons (char *body) {

    return respond(HttpStatusUnavailableForLegalReasons, body);
}

Response
internalServerError (char *body) {

    return respond(HttpStatusInternalServerError, body);
}

Response
notImplemented (char *body) {

    return respond(HttpStatusNotImplemented, body);
}

Response
badGateway (char *body) {

    return respond(HttpStatusBadGateway, body);
}

Response
serviceUnavailable (char *body) {

    return respond(HttpStatusServiceUnavailable, body);
}

Response
gatewayTimeout (char *body) {

    return respond(HttpStatusGatewayTimeout, body);
}

Response
httpVersionNotSupported (char *body) {

    return respond(HttpStatusHttpVersionNotSupported, body);
}

Response
variantAlsoNegotiates (char *body) {

    return respond(HttpStatusVariantAlsoNegotiates, body);
}

Response
insufficientStorage (char *body) {

    return respond(HttpStatusInsufficientStorage, body);
}

Response
loopDetected (char *body) {

    return respond(HttpStatusLoopDetected, body);
}

Response
notExtended (char *body) {

    return respond(HttpStatusNotExtended, body);
}

Response
networkAuthenticationRequired (char *body) {

    return respond(HttpStatusNetworkAuthenticationRequired, body);
}
