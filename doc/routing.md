# Routing

Routing refers to how a client request is to be handled once received.

In Lavender, there are many different ways to declare routes.

<br/>

## Registering Routes

You can register a route by HTTP method, such as with the following.

```c
get(app, get_users);
```

It will create a GET route in your application, under the path '/get_users'.

The other methods similar to this are:

```c
post   (app, create_user);
delete (app, delete_user);
patch  (app, update_user);
put    (app, replace_user);
```

Each of them register a route for a specific HTTP method.

If you follow convention, you can use the following method, which will register a route and decide what HTTP method to use based on the controller name.

```c
use(app, get_users);
```

Since this route starts with 'get', a GET route will be registered.

| Name starts with | Method |
|---|---|
| `get` | GET |
| `create` | POST |
| `update` | PATCH |
| `delete` | DELETE |

Names that start with anything else are registered as GET.

Sometimes, you would prefer to name the route yourself, in which case you can use the following.

```c
at(app, "user_list", get_users);
```

This registers the controller under '/user_list'. Leave out the leading '/', as it is added for you.

The HTTP method for the route will be inferred.

<br/>

## The Root Route

To handle requests to '/', register a GET route with root.

```c
root(app, home);
```

Register it outside of any group.

<br/>

## Route Grouping

To avoid code like the following.

```c
at(app, "users/get_users", get_users);
at(app, "users/create_user", create_user);
at(app, "users/update_user", update_user);
at(app, "users/delete_user", delete_user);
```

You can group routes with a common path.

```c
within (app, "users") {
    get    (app, get_users);
    post   (app, create_user);
    patch  (app, update_user);
    delete (app, delete_user);
}
```

Groups can be nested, and each group adds its path in order.

```c
within (app, "shop") {
    within (app, "orders") {
        // GET /shop/orders/get_orders
        get(app, get_orders);
    }
}
```

<br/>

## Resources

To avoid manually registering the same collection of routes for every object, you can register a resource, which creates four routes for you.

```c
resource(app, user);
```

This is equivalent to the following statements.

```c
route(x, "user", HttpGet, get_user);
route(x, "user", HttpPost, create_user);
route(x, "user", HttpPatch, update_user);
route(x, "user", HttpDelete, delete_user);
```

All four routes share the path '/user', and the HTTP method decides which controller handles the request.

| Method | Path | Controller |
|---|---|---|
| GET | /user | `get_user` |
| POST | /user | `create_user` |
| PATCH | /user | `update_user` |
| DELETE | /user | `delete_user` |

All four controllers must be defined, or the application will not compile.

Resources can be used inside groups, like any other route.

```c
within (app, "api/v1") {
    resource (app, user);    // /api/v1/user
}
```

<br/>

## Registering Routes Manually

If you need more control over a route, such as choosing both its path and its method, you can use 'route'.

```c
route(app, "login", HttpPost, handle_login);
```


Like 'at', the leading '/' is added for you, so the path must be a string literal. 

<br/>


## Viewing Routes

Turning on debug mode prints every registered route when the application starts.

```c
debug(&app, true);
```

```
    GET     /api/v1/user
    POST    /api/v1/user
```
