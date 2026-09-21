# ThreadServe

ThreadServe is a C++20 multithreaded HTTP server built from the ground up. It is designed to demonstrate systems programming fundamentals: TCP networking, HTTP parsing, routing, thread pools, synchronization, timeouts, logging, testing, benchmarking, and Docker deployment.

**Current milestone:** Phase 2 complete. The server creates a TCP socket, binds to `0.0.0.0:8080`, listens, accepts connections, and releases socket resources safely through RAII.

<details>
<summary><strong>Phase 1: Project Foundation</strong></summary>

<a id="phase-1-project-foundation"></a>

### Objective

Create a professional, reproducible C++20 project that can be configured, built, and extended one feature at a time.

### Implemented

- Created a CMake-based project with project metadata and install configuration.
- Enabled standard C++20 and disabled compiler-specific language extensions.
- Added strict warning flags for GCC and Clang: `-Wall`, `-Wextra`, `-Wpedantic`, `-Wconversion`, `-Wsign-conversion`, and `-Wshadow`.
- Added MSVC warning configuration for Windows development.
- Added Debug and Release CMake presets using Ninja.
- Added the first executable target and a minimal `main.cpp` entry point.
- Added repository conventions through `.gitignore`, `.gitattributes`, and `.clang-format`.
- Created clear locations for public headers, tests, benchmarks, and documentation.

### Key Files

```text
CMakeLists.txt       Project, target, C++20, warnings, install rules
CMakePresets.json    Reproducible Debug and Release configurations
src/main.cpp         Initial executable entry point
.gitignore           Build, IDE, and generated-file exclusions
.gitattributes       Consistent line-ending behavior
.clang-format        Shared C++ formatting rules
```

### Validation

```bash
cmake --preset debug
cmake --build --preset debug
./build/debug/threadserve.exe
```

Expected output:

```text
ThreadServe foundation is ready.
```

### Outcome

The project has a reproducible build foundation with modern C++ settings and quality checks ready for networking code.

</details>

<details>
<summary><strong>Phase 2: TCP Listener</strong></summary>

<a id="phase-2-tcp-listener"></a>

### Objective

Build the network entry point that accepts TCP clients and manages socket ownership safely.

### Implemented

- Created an IPv4 TCP socket with `socket()`.
- Configured address reuse with `SO_REUSEADDR`.
- Bound the listener to all local IPv4 interfaces on port `8080`.
- Started listening with a connection backlog of `16`.
- Accepted client connections using `accept()`.
- Added platform-specific implementations for POSIX sockets and Windows Winsock.
- Added `WSAStartup()` and `WSACleanup()` for Windows socket runtime management.
- Added RAII cleanup so socket descriptors close automatically in destructors.
- Deleted copy operations to prevent double ownership of native socket handles.
- Added move construction and move assignment to transfer socket ownership safely.
- Added server lifecycle methods: `start()`, `run()`, and `stop()`.
- Added CMake linking for Windows `ws2_32`.

### Key Files

```text
include/threadserve/tcp_socket.hpp   TcpSocket interface and ownership rules
src/tcp_socket.cpp                   Platform-specific socket implementation
include/threadserve/server.hpp       TcpServer lifecycle interface
src/server.cpp                       Start, accept loop, and stop behavior
src/main.cpp                         Application entry point and error boundary
```

### Runtime Flow

```text
Create socket
    -> Set SO_REUSEADDR
    -> Bind 0.0.0.0:8080
    -> Listen
    -> Accept client
    -> Log connection
    -> Close client socket through RAII
```

### Validation

Build and start the server from MSYS2 UCRT64:

```bash
cmake --preset debug
cmake --build --preset debug
./build/debug/threadserve.exe
```

Expected output:

```text
ThreadServe listening on 0.0.0.0:8080
```

From a second PowerShell terminal:

```powershell
Test-NetConnection 127.0.0.1 -Port 8080
```

Expected result:

```text
TcpTestSucceeded : True
```

Exercise `accept()` with a real client:

```powershell
$client = [System.Net.Sockets.TcpClient]::new()
$client.Connect("127.0.0.1", 8080)
$client.Close()
```

Expected server output:

```text
Accepted TCP connection
```

### Outcome

ThreadServe now has a working TCP lifecycle. It can accept network connections, but it does not parse HTTP or send an HTTP response yet. Accepted connections are intentionally closed until the HTTP response and parser phases are complete.

</details>

<details>
<summary><strong>Phase 3: HTTP Response</strong> (planned)</summary>

<a id="phase-3-http-response"></a>

### Planned Work

- Define an `HttpResponse` model.
- Add status codes and reason phrases.
- Serialize HTTP headers and body.
- Add `Content-Length`.
- Handle partial `send()` operations.
- Send a first valid `HTTP/1.1 200 OK` response over the accepted socket.

</details>

<details>
<summary><strong>Phase 4: HTTP Parser</strong> (planned)</summary>

<a id="phase-4-http-parser"></a>

### Planned Work

- Parse the HTTP request line.
- Parse methods, paths, query strings, and headers.
- Read request bodies using `Content-Length`.
- Reject malformed and oversized requests.
- Add focused parser tests.

</details>

## Requirements

- CMake 3.20 or newer
- A C++20 compiler
- Ninja, recommended with CMake presets
- Linux for the POSIX networking target
- Windows development is supported through MSYS2 UCRT64 and Winsock

The project has been built with GCC 16.2, CMake 4.4.3, and Ninja 1.13.2 in the Windows MSYS2 UCRT64 environment.

## Build and Run

```bash
cmake --preset debug
cmake --build --preset debug
./build/debug/threadserve.exe
```

For a traditional build directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

The current executable listens on `0.0.0.0:8080`. Stop it with `Ctrl+C`.

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
├── tests/
├── benchmarks/
└── docs/
```

## Full Roadmap

1. Project foundation - complete
2. TCP socket layer - complete
3. Basic HTTP response - planned
4. HTTP request parser - planned
5. Blocking queue and thread pool
6. Router
7. Request limits and timeouts
8. Thread-safe logging
9. Graceful shutdown
10. GoogleTest unit and integration tests
11. Google Benchmark benchmarks
12. Docker packaging
13. Documentation and hardening

## License

License information will be added before the first public release.
