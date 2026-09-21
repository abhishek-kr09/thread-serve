# ThreadServe

ThreadServe is a C++20 multithreaded HTTP server built from the ground up. The project will progressively cover TCP sockets, HTTP parsing, routing, thread pools, synchronization, timeouts, logging, testing, benchmarking, Docker, and Linux deployment.

## Current Status

Phase 2 is complete: the server creates a TCP socket, binds to port 8080, listens, accepts connections, and owns socket cleanup through RAII. Accepted connections are currently closed immediately; HTTP handling comes next.

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

The current executable starts a TCP listener on `0.0.0.0:8080` and waits for connections. HTTP request handling will be added in the next phase.

To stop the server, use `Ctrl+C` in the terminal. A graceful programmatic shutdown API is present and will be connected to signal handling in a later phase.

## Test Phase 2

Build and run the server from an MSYS2 UCRT64 terminal:

```bash
cmake --preset debug
cmake --build --preset debug
./build/debug/threadserve.exe
```

In a second terminal, test the TCP port:

```powershell
Test-NetConnection 127.0.0.1 -Port 8080
```

Or from MSYS2:

```bash
printf "test" | nc 127.0.0.1 8080
```

The server should print `Accepted TCP connection`. It will not return an HTTP response yet because HTTP parsing is a later phase.

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
├── include/threadserve/
│   ├── server.hpp
│   └── tcp_socket.hpp
├── src/
│   ├── main.cpp
│   ├── server.cpp
│   └── tcp_socket.cpp
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
