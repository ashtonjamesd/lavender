# Proposals

# Automatic Route Registering

Today, routes are defined as functions, and then later added to a route registry.

```

// my route
approute (route_name) {
    // ..
}

void main () {
    // ..

    // register the route in the app
    get (app, route_name) ;
    
    // ..
}

```

We may be able to register the route at declaration, meaning that the 'get' call can be omitted. This means that we would also have to specify the HTTP method at declaration time too. For instance:

```

getroute (get_user) {
    // ..
}

postroute (create_user) {
    // ..
}

```

We could also opt for just 'get' and 'post' instead of 'getroute', etc, as the 'route' is repetitive. However, those names are already taken by the separate route registering macros, which would have to be renamed.

The 'approute' declaration would now be equivalent to calling 'use' on a route controller, as it would infer from the name of the function.
