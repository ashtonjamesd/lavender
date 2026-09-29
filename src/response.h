#ifndef response_h
#define response_h

#include "common.h"

typedef enum HttpStatus HttpStatus;

#define text_plain "text/plain"
#define application_json "application/json"

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
    const char *body;

    // null means text/plain
    const char *content_type;

};

// sets the content type of a response
Response
with_content_type (Response response, const char *content_type);

// marks a response as json, e.g. json(ok("{\"id\": 1}"))
#define json(response) with_content_type(response, application_json)

Response
respond (HttpStatus status, const char *body);

// 2xx success
Response
ok (const char *body);

Response
created (const char *body);

Response
accepted (const char *body);

Response
nonAuthoritativeInformation (const char *body);

Response
noContent (const char *body);

Response
resetContent (const char *body);

Response
partialContent (const char *body);

Response
multiStatus (const char *body);

Response
alreadyReported (const char *body);

Response
imUsed (const char *body);

// 3xx redirection
Response
multipleChoices (const char *body);

Response
movedPermanently (const char *body);

Response
found (const char *body);

Response
seeOther (const char *body);

Response
notModified (const char *body);

Response
temporaryRedirect (const char *body);

Response
permanentRedirect (const char *body);

// 4xx client errors
Response
badRequest (const char *body);

Response
unauthorized (const char *body);

Response
paymentRequired (const char *body);

Response
forbidden (const char *body);

Response
notFound (const char *body);

Response
methodNotAllowed (const char *body);

Response
notAcceptable (const char *body);

Response
proxyAuthenticationRequired (const char *body);

Response
requestTimeout (const char *body);

Response
conflict (const char *body);

Response
gone (const char *body);

Response
lengthRequired (const char *body);

Response
preconditionFailed (const char *body);

Response
contentTooLarge (const char *body);

Response
uriTooLong (const char *body);

Response
unsupportedMediaType (const char *body);

Response
rangeNotSatisfiable (const char *body);

Response
expectationFailed (const char *body);

Response
imATeapot (const char *body);

Response
misdirectedRequest (const char *body);

Response
unprocessableContent (const char *body);

Response
locked (const char *body);

Response
failedDependency (const char *body);

Response
tooEarly (const char *body);

Response
upgradeRequired (const char *body);

Response
preconditionRequired (const char *body);

Response
tooManyRequests (const char *body);

Response
requestHeaderFieldsTooLarge (const char *body);

Response
unavailableForLegalReasons (const char *body);

// 5xx server errors
Response
internalServerError (const char *body);

Response
notImplemented (const char *body);

Response
badGateway (const char *body);

Response
serviceUnavailable (const char *body);

Response
gatewayTimeout (const char *body);

Response
httpVersionNotSupported (const char *body);

Response
variantAlsoNegotiates (const char *body);

Response
insufficientStorage (const char *body);

Response
loopDetected (const char *body);

Response
notExtended (const char *body);

Response
networkAuthenticationRequired (const char *body);

#endif
