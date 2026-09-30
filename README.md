![Lavender](lavender_text.png)

A C web framework.

<br/>

> **Warning:**
> Lavender is early, experimental, and a WIP. Expect missing features, bugs, and breaking changes to the API.

## Example

```c
#include "lavender.h"

middleware (require_key) {
    const char *key = header("X-Api-Key");

    return (!safe_str_eq(key, "KEY"))
        ? unauthorized("no key, no entry")
        : next();
}

approute (hello) {
    return ok("Hello World!");
}

approute (get_secret) {
    return ok("lavender smells nice");
}

int main() {
    App x = app(3000);

    within (x, "api/v1") {
        get(x, hello);

        guarded (x, require_key) {
            use(x, get_secret);
        }
    }

    run(&x);

    return 0;
}

```

```sh
$ curl localhost:3000/api/v1/hello
Hello World!

$ curl localhost:3000/api/v1/get_secret
no key, no entry

$ curl -H "X-Api-Key: KEY" localhost:3000/api/v1/get_secret
{"secret":"lavender smells nice"}
```

More examples are in [example/](example/).

## Building

```sh
make            # build
make test       # run tests
make examples   # build the examples
```

## Dependencies

Lavender requires the following.

- sqlite3
- libmicrohttpd
- yyjson

## Documentation

- [Routing](doc/routing.md)
- [Guards](doc/guards.md)
- [Coding Standards](doc/coding_practices.md)

## Contributing

Contributions are welcome. Before opening a pull request:

- Follow the [coding standards](doc/coding_practices.md).
- Ensure all tests are passing.

## License

[SQLite Blessing](LICENSE)
