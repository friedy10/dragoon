cmake_minimum_required(VERSION 3.10)
project(dragoon)

# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the source files to the project
set(SOURCES
    client.cpp
    gdbmi.cpp
    gdbmi.hpp
    wxStringHash.h
)

# Create an executable from the sources
add_executable(dragoon ${SOURCES})
