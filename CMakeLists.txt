cmake_minimum_required(VERSION 3.6.0)

project("Fast_Division")

option(FAST_DIVISION_BUILD_EXAMPLE "Build fast division example application?" OFF)
option(FAST_DIVISION_BUILD_TESTS "Build fast division tests?" OFF)

include_directories("include")

if(FAST_DIVISION_BUILD_EXAMPLE)
    add_subdirectory("example")
endif()

if(FAST_DIVISION_BUILD_TESTS)
    add_subdirectory("tests")
endif()

add_library(fast_division INTERFACE)

set(FAST_DIVISION_SOURCES include/fast_division.hpp)
target_include_directories(fast_division INTERFACE include)
target_sources(fast_division INTERFACE ${FAST_DIVISION_SOURCES})
source_group(fast_division ${FAST_DIVISION_SOURCES})

# Hackish way to add the library to IDEs
add_custom_target(fast_division_library SOURCES ${FAST_DIVISION_SOURCES})