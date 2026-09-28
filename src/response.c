#include "response.h"

Response
respond (HttpStatus status, cString body) {

    return (Response) {
        .status = status,
        .body = body,
    };
}

Response
ok (cString body) {

    return respond(HttpStatusOk, body);
}

Response
created (cString body) {

    return respond(HttpStatusCreated, body);
}

Response
accepted (cString body) {

    return respond(HttpStatusAccepted, body);
}

Response
nonAuthoritativeInformation (cString body) {

    return respond(HttpStatusNonAuthoritativeInformation, body);
}

Response
noContent (cString body) {

    return respond(HttpStatusNoContent, body);
}

Response
resetContent (cString body) {

    return respond(HttpStatusResetContent, body);
}

Response
partialContent (cString body) {

    return respond(HttpStatusPartialContent, body);
}

Response
multiStatus (cString body) {

    return respond(HttpStatusMultiStatus, body);
}

Response
alreadyReported (cString body) {

    return respond(HttpStatusAlreadyReported, body);
}

Response
imUsed (cString body) {

    return respond(HttpStatusImUsed, body);
}

Response
multipleChoices (cString body) {

    return respond(HttpStatusMultipleChoices, body);
}

Response
movedPermanently (cString body) {

    return respond(HttpStatusMovedPermanently, body);
}

Response
found (cString body) {

    return respond(HttpStatusFound, body);
}

Response
seeOther (cString body) {

    return respond(HttpStatusSeeOther, body);
}

Response
notModified (cString body) {

    return respond(HttpStatusNotModified, body);
}

Response
temporaryRedirect (cString body) {

    return respond(HttpStatusTemporaryRedirect, body);
}

Response
permanentRedirect (cString body) {

    return respond(HttpStatusPermanentRedirect, body);
}

Response
badRequest (cString body) {

    return respond(HttpStatusBadRequest, body);
}

Response
unauthorized (cString body) {

    return respond(HttpStatusUnauthorized, body);
}

Response
paymentRequired (cString body) {

    return respond(HttpStatusPaymentRequired, body);
}

Response
forbidden (cString body) {

    return respond(HttpStatusForbidden, body);
}

Response
notFound (cString body) {

    return respond(HttpStatusNotFound, body);
}

Response
methodNotAllowed (cString body) {

    return respond(HttpStatusMethodNotAllowed, body);
}

Response
notAcceptable (cString body) {

    return respond(HttpStatusNotAcceptable, body);
}

Response
proxyAuthenticationRequired (cString body) {

    return respond(HttpStatusProxyAuthenticationRequired, body);
}

Response
requestTimeout (cString body) {

    return respond(HttpStatusRequestTimeout, body);
}

Response
conflict (cString body) {

    return respond(HttpStatusConflict, body);
}

Response
gone (cString body) {

    return respond(HttpStatusGone, body);
}

Response
lengthRequired (cString body) {

    return respond(HttpStatusLengthRequired, body);
}

Response
preconditionFailed (cString body) {

    return respond(HttpStatusPreconditionFailed, body);
}

Response
contentTooLarge (cString body) {

    return respond(HttpStatusContentTooLarge, body);
}

Response
uriTooLong (cString body) {

    return respond(HttpStatusUriTooLong, body);
}

Response
unsupportedMediaType (cString body) {

    return respond(HttpStatusUnsupportedMediaType, body);
}

Response
rangeNotSatisfiable (cString body) {

    return respond(HttpStatusRangeNotSatisfiable, body);
}

Response
expectationFailed (cString body) {

    return respond(HttpStatusExpectationFailed, body);
}

Response
imATeapot (cString body) {

    return respond(HttpStatusImATeapot, body);
}

Response
misdirectedRequest (cString body) {

    return respond(HttpStatusMisdirectedRequest, body);
}

Response
unprocessableContent (cString body) {

    return respond(HttpStatusUnprocessableContent, body);
}

Response
locked (cString body) {

    return respond(HttpStatusLocked, body);
}

Response
failedDependency (cString body) {

    return respond(HttpStatusFailedDependency, body);
}

Response
tooEarly (cString body) {

    return respond(HttpStatusTooEarly, body);
}

Response
upgradeRequired (cString body) {

    return respond(HttpStatusUpgradeRequired, body);
}

Response
preconditionRequired (cString body) {

    return respond(HttpStatusPreconditionRequired, body);
}

Response
tooManyRequests (cString body) {

    return respond(HttpStatusTooManyRequests, body);
}

Response
requestHeaderFieldsTooLarge (cString body) {

    return respond(HttpStatusRequestHeaderFieldsTooLarge, body);
}

Response
unavailableForLegalReasons (cString body) {

    return respond(HttpStatusUnavailableForLegalReasons, body);
}

Response
internalServerError (cString body) {

    return respond(HttpStatusInternalServerError, body);
}

Response
notImplemented (cString body) {

    return respond(HttpStatusNotImplemented, body);
}

Response
badGateway (cString body) {

    return respond(HttpStatusBadGateway, body);
}

Response
serviceUnavailable (cString body) {

    return respond(HttpStatusServiceUnavailable, body);
}

Response
gatewayTimeout (cString body) {

    return respond(HttpStatusGatewayTimeout, body);
}

Response
httpVersionNotSupported (cString body) {

    return respond(HttpStatusHttpVersionNotSupported, body);
}

Response
variantAlsoNegotiates (cString body) {

    return respond(HttpStatusVariantAlsoNegotiates, body);
}

Response
insufficientStorage (cString body) {

    return respond(HttpStatusInsufficientStorage, body);
}

Response
loopDetected (cString body) {

    return respond(HttpStatusLoopDetected, body);
}

Response
notExtended (cString body) {

    return respond(HttpStatusNotExtended, body);
}

Response
networkAuthenticationRequired (cString body) {

    return respond(HttpStatusNetworkAuthenticationRequired, body);
}
