![Lavender](lavender_text.png)

A C web framework.

<br/>

> **Warning:**
> Lavender is early and experimental. Expect missing features, bugs, and breaking changes to the API.

## Example

```c
#include "lavender.h"

approute (home) {
    return ok("Hello World!");
}

int main() {
    App x = app(3000);

    within (x, "api/v1") {
        use(x, home);
    }

    run(x);

    return 0;
}
```

```sh
$ curl localhost:3000/api/v1/home
Hello World!
```

More examples are in [example/](example/).

## Building

Lavender requires [libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/).

```sh
make            # build
make test       # run tests
make examples   # build the examples
```

## Documentation

- [Routing](doc/routing.md)
- [Coding Standards](doc/coding_practices.md)

## Contributing

Contributions are welcome. Before opening a pull request:

- Follow the [coding standards](doc/coding_practices.md).
- Ensure all tests are passing.

## License

[SQLite Blessing](LICENSE)
