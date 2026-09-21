cmake_minimum_required(VERSION 3.20)

project(ThreadServe
    VERSION 0.1.0
    DESCRIPTION "A C++20 multithreaded HTTP server"
    LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(threadserve
    src/main.cpp
)

target_compile_features(threadserve PRIVATE cxx_std_20)

target_compile_options(threadserve PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wsign-conversion;-Wshadow>
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/permissive->
)

include(GNUInstallDirs)
install(TARGETS threadserve
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
