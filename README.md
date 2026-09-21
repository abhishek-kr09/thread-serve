# ThreadServe

ThreadServe is a C++20 multithreaded HTTP server built from the ground up. The project will progressively cover TCP sockets, HTTP parsing, routing, thread pools, synchronization, timeouts, logging, testing, benchmarking, Docker, and Linux deployment.

## Current Status

Phase 1 is complete: the repository has a CMake foundation, C++20 compiler settings, warning flags, a basic executable, and this README scaffold.

## Requirements

- CMake 3.20 or newer
- A C++20 compiler
- Ninja (recommended when using CMake presets)
- Linux for the networking phases

Supported compilers include recent GCC and Clang releases. MSVC is supported for the foundation build, but the server networking implementation will target POSIX/Linux APIs.

## Build

Using CMake presets:

```bash
cmake --preset debug
cmake --build --preset debug
```

Using a traditional build directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

## Run

Linux/macOS:

```bash
./build/debug/threadserve
```

Traditional build directory:

```bash
./build/threadserve
```

Windows with a Visual Studio generator:

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\threadserve.exe
```

The current executable only verifies the project foundation and prints a startup message. TCP listening will be added in Phase 2.

## Planned Architecture

```text
HTTP client
    |
    v
TCP listener -> accept loop -> bounded work queue
                                  |
                                  v
                         thread pool workers
                                  |
                                  v
                 HTTP parser -> router -> handler
                                  |
                                  v
                         HTTP response writer
```

Cross-cutting components will provide configuration, timeouts, logging, and graceful shutdown.

## Project Structure

```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── src/
│   └── main.cpp
├── include/threadserve/
├── tests/
├── benchmarks/
└── docs/
```

The empty component directories will be populated one feature at a time.

## Development Phases

1. Project foundation
2. TCP socket layer
3. Basic HTTP response
4. HTTP request parser
5. Blocking queue and thread pool
6. Router
7. Request limits and timeouts
8. Thread-safe logging
9. Graceful shutdown
10. GoogleTest unit and integration tests
11. Google Benchmark benchmarks
12. Docker packaging
13. Documentation and hardening

## Testing

Automated tests will be enabled when the first independently testable component is added. For now, validate the foundation by configuring, building, and running the executable.

## License

License information will be added before the first public release.
