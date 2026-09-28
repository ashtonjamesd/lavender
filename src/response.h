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
    char *body;

};

Response
respond (HttpStatus status, char *body);

// 2xx success
Response
ok (char *body);

Response
created (char *body);

Response
accepted (char *body);

Response
nonAuthoritativeInformation (char *body);

Response
noContent (char *body);

Response
resetContent (char *body);

Response
partialContent (char *body);

Response
multiStatus (char *body);

Response
alreadyReported (char *body);

Response
imUsed (char *body);

// 3xx redirection
Response
multipleChoices (char *body);

Response
movedPermanently (char *body);

Response
found (char *body);

Response
seeOther (char *body);

Response
notModified (char *body);

Response
temporaryRedirect (char *body);

Response
permanentRedirect (char *body);

// 4xx client errors
Response
badRequest (char *body);

Response
unauthorized (char *body);

Response
paymentRequired (char *body);

Response
forbidden (char *body);

Response
notFound (char *body);

Response
methodNotAllowed (char *body);

Response
notAcceptable (char *body);

Response
proxyAuthenticationRequired (char *body);

Response
requestTimeout (char *body);

Response
conflict (char *body);

Response
gone (char *body);

Response
lengthRequired (char *body);

Response
preconditionFailed (char *body);

Response
contentTooLarge (char *body);

Response
uriTooLong (char *body);

Response
unsupportedMediaType (char *body);

Response
rangeNotSatisfiable (char *body);

Response
expectationFailed (char *body);

Response
imATeapot (char *body);

Response
misdirectedRequest (char *body);

Response
unprocessableContent (char *body);

Response
locked (char *body);

Response
failedDependency (char *body);

Response
tooEarly (char *body);

Response
upgradeRequired (char *body);

Response
preconditionRequired (char *body);

Response
tooManyRequests (char *body);

Response
requestHeaderFieldsTooLarge (char *body);

Response
unavailableForLegalReasons (char *body);

// 5xx server errors
Response
internalServerError (char *body);

Response
notImplemented (char *body);

Response
badGateway (char *body);

Response
serviceUnavailable (char *body);

Response
gatewayTimeout (char *body);

Response
httpVersionNotSupported (char *body);

Response
variantAlsoNegotiates (char *body);

Response
insufficientStorage (char *body);

Response
loopDetected (char *body);

Response
notExtended (char *body);

Response
networkAuthenticationRequired (char *body);

#endif
