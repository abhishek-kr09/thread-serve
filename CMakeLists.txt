cmake_minimum_required(VERSION 3.20)

project(ThreadServe
    VERSION 0.1.0
    DESCRIPTION "A C++20 multithreaded HTTP server"
    LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(threadserve
    src/main.cpp
    src/server.cpp
    src/tcp_socket.cpp
)

target_compile_features(threadserve PRIVATE cxx_std_20)

target_include_directories(threadserve PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(threadserve PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wsign-conversion;-Wshadow>
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/permissive->
)

if(WIN32)
    target_link_libraries(threadserve PRIVATE ws2_32)
endif()

include(GNUInstallDirs)
install(TARGETS threadserve
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
