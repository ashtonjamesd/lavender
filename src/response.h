#ifndef response_h
#define response_h

#include "common.h"

typedef enum HttpStatus HttpStatus;

enum HttpStatus {
    // 2xx success
    HttpStatusOk = 200,
    HttpStatusCreated = 201,
    HttpStatusAccepted = 202,
    HttpStatusNonAuthoritativeInformation = 203,
    HttpStatusNoContent = 204,
    HttpStatusResetContent = 205,
    HttpStatusPartialContent = 206,
    HttpStatusMultiStatus = 207,
    HttpStatusAlreadyReported = 208,
    HttpStatusImUsed = 226,

    // 3xx redirection
    HttpStatusMultipleChoices = 300,
    HttpStatusMovedPermanently = 301,
    HttpStatusFound = 302,
    HttpStatusSeeOther = 303,
    HttpStatusNotModified = 304,
    
    // depreciated
    // HttpStatusUseProxy = 305,
    
    HttpStatusTemporaryRedirect = 307,
    HttpStatusPermanentRedirect = 308,

    // 4xx client errors
    HttpStatusBadRequest = 400,
    HttpStatusUnauthorized = 401,
    HttpStatusPaymentRequired = 402,
    HttpStatusForbidden = 403,
    HttpStatusNotFound = 404,
    HttpStatusMethodNotAllowed = 405,
    HttpStatusNotAcceptable = 406,
    HttpStatusProxyAuthenticationRequired = 407,
    HttpStatusRequestTimeout = 408,
    HttpStatusConflict = 409,
    HttpStatusGone = 410,
    HttpStatusLengthRequired = 411,
    HttpStatusPreconditionFailed = 412,
    HttpStatusContentTooLarge = 413,
    HttpStatusUriTooLong = 414,
    HttpStatusUnsupportedMediaType = 415,
    HttpStatusRangeNotSatisfiable = 416,
    HttpStatusExpectationFailed = 417,
    HttpStatusImATeapot = 418,
    HttpStatusMisdirectedRequest = 421,
    HttpStatusUnprocessableContent = 422,
    HttpStatusLocked = 423,
    HttpStatusFailedDependency = 424,
    HttpStatusTooEarly = 425,
    HttpStatusUpgradeRequired = 426,
    HttpStatusPreconditionRequired = 428,
    HttpStatusTooManyRequests = 429,
    HttpStatusRequestHeaderFieldsTooLarge = 431,
    HttpStatusUnavailableForLegalReasons = 451,

    // 5xx server errors
    HttpStatusInternalServerError = 500,
    HttpStatusNotImplemented = 501,
    HttpStatusBadGateway = 502,
    HttpStatusServiceUnavailable = 503,
    HttpStatusGatewayTimeout = 504,
    HttpStatusHttpVersionNotSupported = 505,
    HttpStatusVariantAlsoNegotiates = 506,
    HttpStatusInsufficientStorage = 507,
    HttpStatusLoopDetected = 508,
    HttpStatusNotExtended = 510,
    HttpStatusNetworkAuthenticationRequired = 511,
};

typedef struct Response Response;

struct Response {
    HttpStatus status;
    cString body;

};

// builds a response with any status code
Response
respond (HttpStatus status, cString body);

// 2xx success
Response
ok (cString body);

Response
created (cString body);

Response
accepted (cString body);

Response
nonAuthoritativeInformation (cString body);

Response
noContent (cString body);

Response
resetContent (cString body);

Response
partialContent (cString body);

Response
multiStatus (cString body);

Response
alreadyReported (cString body);

Response
imUsed (cString body);

// 3xx redirection
Response
multipleChoices (cString body);

Response
movedPermanently (cString body);

Response
found (cString body);

Response
seeOther (cString body);

Response
notModified (cString body);

Response
temporaryRedirect (cString body);

Response
permanentRedirect (cString body);

// 4xx client errors
Response
badRequest (cString body);

Response
unauthorized (cString body);

Response
paymentRequired (cString body);

Response
forbidden (cString body);

Response
notFound (cString body);

Response
methodNotAllowed (cString body);

Response
notAcceptable (cString body);

Response
proxyAuthenticationRequired (cString body);

Response
requestTimeout (cString body);

Response
conflict (cString body);

Response
gone (cString body);

Response
lengthRequired (cString body);

Response
preconditionFailed (cString body);

Response
contentTooLarge (cString body);

Response
uriTooLong (cString body);

Response
unsupportedMediaType (cString body);

Response
rangeNotSatisfiable (cString body);

Response
expectationFailed (cString body);

Response
imATeapot (cString body);

Response
misdirectedRequest (cString body);

Response
unprocessableContent (cString body);

Response
locked (cString body);

Response
failedDependency (cString body);

Response
tooEarly (cString body);

Response
upgradeRequired (cString body);

Response
preconditionRequired (cString body);

Response
tooManyRequests (cString body);

Response
requestHeaderFieldsTooLarge (cString body);

Response
unavailableForLegalReasons (cString body);

// 5xx server errors
Response
internalServerError (cString body);

Response
notImplemented (cString body);

Response
badGateway (cString body);

Response
serviceUnavailable (cString body);

Response
gatewayTimeout (cString body);

Response
httpVersionNotSupported (cString body);

Response
variantAlsoNegotiates (cString body);

Response
insufficientStorage (cString body);

Response
loopDetected (cString body);

Response
notExtended (cString body);

Response
networkAuthenticationRequired (cString body);

#endif
