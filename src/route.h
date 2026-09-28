#ifndef route_h
#define route_h

#include "common.h"
#include "str.h"

#include "request.h"
#include "response.h"

typedef Response (*Controller)(Request);

typedef struct Route Route;

struct Route {
    string path;
    HttpType type;

    Controller controller;

};

#endif