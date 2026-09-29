# Coding Standards

## 1 Functions

Function declarations and definitions are written as follows:

- Function qualifiers and return types are on one separate line
- A single space follows the function name, before the parentheses
- Left brace starts on the function signature line, and the right on a separate newline
- Two newline characters must follow the left brace
- Functions are written in camelCase
- Parameters are standard and written as shown

<br>

```
static void
myFunc (int p1, cString p2, float p3);

static void
myFunc (int p1, cString p2, float p3) {

    // code starts here 
}
```

## 2 Keyword Usage

- Prefer keywords 'and' and 'or' from header: 'iso646', over their related symbol.
- Prefer usage of 'null' over 'NULL'


## 3 Includes

Given a file 'foo.c', with a related header 'foo.h', the including of the header file must be on line 1 of the source file followed by two newlines.

All files will include 'common.h' at the beginning.


## 4 Structs

- Structs are always typedef'd and they must be separate from the struct body definition.
- The last field in a struct must be followed by two newlines.


## 5 Other

- The typedefs and macros inside of 'common.h' shall be preferred over standard C types, wherever possible.
- The bang (!) operator should not be used for null checks; use explicit '== null' and '!= null'.
- Prefer '+= 1' over '++' and '-= 1' over '--'.
- Any function part of the user API must have a short comment explaining the usage.
- Special exceptions to any of these will be considered case-by-case

## 6 Coding Philosophies

1. Have fun
2. Prioritise user experience and brevity without sacrificing clarity when designing the framework API
3. Every pull request must conclude with a kind message or fun joke
