cmake_minimum_required(VERSION 3.20.2)
project(dragoon)

include(CMakePrintHelpers)

# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the source files to the project
set(SOURCES
    client.cpp
    gdbmi.cpp
    gdbmi.hpp
)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

include_directories(/usr/local/opt/openssl@3.1/include)
link_directories(/usr/local/opt/openssl@3.1/lib)

find_package(LibSSH REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${LIBSSH_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIRS})


cmake_print_variables(OPENSSL_LIBRARY)

add_executable(dragoon ${SOURCES})
target_link_libraries(dragoon PRIVATE crypto)
target_link_libraries(dragoon PRIVATE ssh)
