# Guards

Guards are functions that run before a controller, and decide whether a request is allowed to reach it.

In Lavender, guards are useful for checks shared between many routes, such as authentication or logging.

<br/>

## Defining Guards

You can define a guard with 'middleware', in the same way as 'approute'.

```c
middleware (require_login) {

    if (header("Authorization") == null) {
        return unauthorized("please log in");
    }

    return next();
}
```

Returning 'next' lets the request carry on, and returning any other response stops it.

<br/>

## Adding Guards

You can add a guard to your application with the following.

```c
guard(app, require_login);
```

It will guard every route registered after it, and routes registered before it are not affected.

```c
get(app, home);             // not guarded

guard(app, require_login);

get(app, get_profile);      // guarded
```

<br/>

## Guard Grouping

A guard added inside of a group only applies to routes in that group.

```c
within (app, "admin") {
    guard(app, require_admin);

    // GET /admin/get_users
    get(app, get_users);
}

// not guarded
get(app, home);
```

Groups can be nested, and each group adds its guards in order.

```c
within (app, "api") {
    guard(app, require_login);

    within (app, "admin") {
        guard(app, require_admin);

        // require_login, then require_admin
        get(app, delete_post);
    }

    // require_login only
    get(app, get_posts);
}
```

A guard added outside of any group applies to every route registered after it, including those inside of groups.

<br/>

## Guarded Blocks

To guard a few routes without giving them a common path, you can use 'guarded'.

```c
guarded (app, require_login) {
    // GET /get_profile
    get(app, get_profile);
}

// not guarded
get(app, home);
```

The guard only applies to routes inside of the block. Guarded blocks can also be used inside of groups, and the routes keep the group's path.

<br/>

## Guard Order

Guards run in the order they were added.

```c
guard(app, log_request);
guard(app, require_login);
guard(app, rate_limit);

get(app, get_profile);
```

This is the order each request will follow.

```
log_request -> require_login -> rate_limit -> get_profile
```

If 'require_login' does not return 'next', then 'rate_limit' and 'get_profile' will not run.

<br/>

## Leaving Groups Early

Do not use 'return', 'break' or 'goto' inside of a group or guarded block.

Leaving one early means it is never ended, so its guards would apply to every route registered afterwards. To catch this, the application will panic when it is run.

<br/>
