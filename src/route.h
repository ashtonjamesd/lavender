#ifndef route_h
#define route_h

#include "common.h"
#include "strings.h"

#include "request.h"
#include "response.h"

typedef enum HttpType HttpType;

enum HttpType {
    HttpGet,
    HttpPost,
    HttpPatch,
    HttpPut,
    HttpDelete,
    HttpOptions,
};

typedef Response (*Controller)(Request);

typedef struct Route Route;

struct Route {
    string path;
    HttpType type;
    Controller controller;

};

#endif