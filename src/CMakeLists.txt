cmake_minimum_required(VERSION 3.20.2)
project(dragoon)


# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CMakePrintHelpers)

# Add the source files to the project
set(SOURCES
    client.cpp
    gdbmi.cpp
    gdbmi.hpp
    client.hpp
    client.cpp
)


set(GDBMI_TEST_SRCS
    gdbmi_test.cpp
    gdbmi.cpp
    gdbmi.hpp
)

set(NETWORK_SRCS
    network.cpp
    network.hpp
)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

include_directories(/usr/local/opt/openssl@3.1/include)
link_directories(/usr/local/opt/openssl@3.1/lib)

find_package(LibSSH REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PcapPlusPlus REQUIRED)
#find_package(PCAP REQUIRED)

include_directories(${LIBSSH_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIRS})
include_directories(${PCAP_INCLUDE_DIR})


cmake_print_variables(OPENSSL_LIBRARY)

add_executable(dragoon ${SOURCES})
add_executable(gdbmi_test ${GDBMI_TEST_SRCS})
add_executable(dragoon_network ${NETWORK_SRCS})

# Libs for dragoon
target_link_libraries(dragoon PRIVATE crypto)
#target_link_libraries(dragoon PRIVATE ${PCAP_LIBRARY})
target_link_libraries(dragoon PRIVATE ssh)

# Libs for dragoon_network
target_link_libraries(dragoon_network PUBLIC PcapPlusPlus::Pcap++)
