# command-line-calculator

A simple command-line calculator that evaluates a math expression passed as a single command-line argument.

Supports:

- Numbers (integers and decimals)
- Operators: `+`, `-`, `*`, `/`
- Parentheses: `(`, `)`
- Unary minus (e.g. `-3`, `-(2+1)`)

## Build

Requires a C++17 compiler and CMake.

You do not need to run a separate build step if you use `./toy-app` — it will automatically configure/build into `./build/` on first run (if needed) and then execute the compiled binary.

### Quick (recommended)

If you have `make` (default on macOS):

```bash
make build
```

### CMake (manual)

```bash
cmake -S . -B build
cmake --build build
```

## Run

### One-shot

```bash
./toy-app "(2+5)*2"
```

Note: `./toy-app` is a small launcher script that builds the CMake binary into `./build/` on first run (if needed), then executes it.

Or via `make`:

```bash
make run EXPR="(2+5)*2"
```

Output:

```text
14
```

### Live calculator (REPL)

Run with no arguments to enter interactive mode:

```bash
./toy-app
```

## Tests

```bash
make test
```

Or, with CMake directly:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```
